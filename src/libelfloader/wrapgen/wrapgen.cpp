#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sstream>
#include <stdexcept>
#include <fstream>

#ifndef PATH_MAX
#	define PATH_MAX	4096
#endif

// This is a generator of assembly code for Mach-O
// libraries wrapping ELF libraries.

// TODO: use wrapgen32 to generate 32-bit wrappers.

void parse_elf(const char* elf, std::string& soname_out, std::set<std::string>& symbols_out);
void generate_wrapper(std::ofstream& output, const char* soname, const std::set<std::string>& symbols, const std::vector<std::string>& dependencies);

std::string get_library_path(std::string elfLibrary)
{
    if (access(elfLibrary.c_str(), R_OK) == -1)
	{
		// Try loading the library and then ask the loader where it found the library.
		// It is simpler than parsing /etc/ld.so.conf.

		void* handle = dlopen(elfLibrary.c_str(), RTLD_LAZY | RTLD_LOCAL);
		char path[PATH_MAX];

		if (!handle)
		{
			std::stringstream ss;
			ss << "Cannot load " << elfLibrary << ": " << dlerror();
			throw std::runtime_error(ss.str());
		}

		if (dlinfo(handle, RTLD_DI_ORIGIN, path) == 0)
		{
			elfLibrary.insert(0, "/");
			elfLibrary.insert(0, path);
		}
		else
		{
			std::stringstream ss;
			std::cerr << "Cannot locate " << elfLibrary << ": " << dlerror();
			throw std::runtime_error(ss.str());
		}

		dlclose(handle);
	}
	return elfLibrary;
}

int main(int argc, const char** argv)
{
	std::string elfLibrary;
	std::set<std::string> symbols;
	std::vector<std::string> dependencies;
	std::string soname;
	std::ofstream output;

	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <library-name> <output-file> [library-non-standard-dependencies]\n";
		return 1;
	}

	elfLibrary = argv[1];
	output.open(argv[2]);

	try
	{
		if (!output.is_open())
			throw std::runtime_error("Cannot open output file");

		elfLibrary = get_library_path(elfLibrary);
		for (int i = 3; i < argc; i++)
		{
		    // The Depedencies argument is for ELF depedencies thatare located in a non-standard location, other depedencies are found automatically.
		    // Because they are stored in a non-standard location they must be an absolute path, and therefore must have /Volumes/SystemRoot appeneed to the front so Darling can find it.
		    // This is need because RPath doesn't work because all of the libraries are in /Volumes/SystemRoot instead of /.
		    dependencies.push_back(std::string(argv[i]).insert(0, "/Volumes/SystemRoot"));
		}

		parse_elf(elfLibrary.c_str(), soname, symbols);
		generate_wrapper(output, soname.c_str(), symbols, dependencies);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void parse_elf(const char* elf, std::string& soname, std::set<std::string>& symbols)
{
	int fd;
	const Elf64_Ehdr* ehdr;
	const char* strings = NULL;
	uint64_t length;

	fd = open(elf, O_RDONLY);
	if (fd < 0)
	{
		std::stringstream ss;
		ss << "Error opening " << elf << ": " << strerror(errno);
		throw std::runtime_error(ss.str());
	}

	length = lseek(fd, 0, SEEK_END);

	ehdr = (Elf64_Ehdr*) mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
	if (ehdr == MAP_FAILED)
	{
		std::stringstream ss;
		ss << "Cannot mmap ELF file: " << strerror(errno);
		throw std::runtime_error(ss.str());
	}

	close(fd);

	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0 || ehdr->e_ident[EI_CLASS] != ELFCLASS64)
	{
		std::stringstream ss;
		ss << elf << " is not a 64-bit ELF";
		throw std::runtime_error(ss.str());
	}

	if (ehdr->e_type != ET_DYN)
	{
		std::stringstream ss;
		ss << elf << " is not a dynamic library ELF";
		throw std::runtime_error(ss.str());
	}

	if (ehdr->e_machine != EM_X86_64)
	{
		std::stringstream ss;
		ss << elf << " is not an ELF for x86-64";
		throw std::runtime_error(ss.str());
	}

	// Now read program headers, find a PT_DYNAMIC segment type
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		const Elf64_Phdr* phdr;

		phdr = (const Elf64_Phdr*) (((char*) ehdr) + ehdr->e_phoff + (i * ehdr->e_phentsize));

		if (phdr->p_type == PT_DYNAMIC)
		{
			Elf64_Xword off_strtab, off_soname;

			off_strtab = off_soname = 0;

			for (int j = 0; j < phdr->p_filesz; j += sizeof(Elf64_Dyn))
			{
				const Elf64_Dyn* dyn;

				dyn = (const Elf64_Dyn*) (((char*) ehdr) + phdr->p_offset + j);
				
				switch (dyn->d_tag)
				{
					case DT_STRTAB:
						off_strtab = dyn->d_un.d_val;
						break;
					case DT_SONAME:
						off_soname = dyn->d_un.d_val;
						break;
					case DT_NULL:
						goto end_dyn;
				}
			}
end_dyn:

			if (off_strtab != 0)
			{
				strings = (const char*) ((char*) ehdr) + off_strtab;

				if (off_soname != 0)
				{
					// Read DT_SONAME value from the string table
					soname = strings + off_soname;
				}
			}

			break;
		}
	}

	if (strings != NULL)
	{
		// Load symbol list
		const Elf64_Shdr* shdr;

		for (int i = 0; i < ehdr->e_shnum; i++)
		{
			shdr = (const Elf64_Shdr*) (((char*) ehdr) + ehdr->e_shoff + (i * ehdr->e_shentsize));
			if (shdr->sh_type == SHT_DYNSYM)
			{
				for (int j = 0; j < shdr->sh_size; j += sizeof(Elf64_Sym))
				{
					const Elf64_Sym* sym;

					sym = (const Elf64_Sym*) (((char*) ehdr) + shdr->sh_offset + j);

					if (ELF64_ST_TYPE(sym->st_info) != STT_OBJECT && ELF64_ST_TYPE(sym->st_info) != STT_FUNC)
						continue;
					if (ELF64_ST_BIND(sym->st_info) != STB_GLOBAL)
						continue;
					if (sym->st_shndx == SHN_UNDEF || sym->st_value == 0)
						continue;
					if (ELF64_ST_VISIBILITY(sym->st_other) != STV_DEFAULT)
						continue;

					symbols.insert(strings + sym->st_name);
				}

				break;
			}
		}
	}

	if (soname.empty())
	{
		std::cerr << "WARNING: No DT_SONAME in " << elf << ".\n";

		const char* slash = strrchr(elf, '/');
		if (slash != NULL)
			soname = slash + 1;
		else
			soname = elf;
	}
	if (symbols.empty())
	{
		std::stringstream ss;
		ss << "No symbols found in " << elf;
		throw std::runtime_error(ss.str());
	}

	munmap((void*) ehdr, length);
}

void generate_wrapper(std::ofstream& output, const char* soname, const std::set<std::string>& symbols, const std::vector<std::string>& dependencies)
{
	output << "#include <elfcalls.h>\n"
		"extern struct elf_calls* _elfcalls;\n\n"
		"extern const char __elfname[];\n\n";

    for (int i = 0; i < dependencies.size(); i++)
    {
	    output << "extern const char __elfname_" << i << "[];\n\n"
	        "static void* lib_" << i << "_handle;\n"
		    "static void initializer_" << i << "() {\n"
		    "\tlib_" << i << "_handle = _elfcalls->dlopen_fatal(__elfname_" << i << ");\n"
		    "}\n\n";
    }

	output << "static void* lib_handle;\n"
		"__attribute__((constructor)) static void initializer() {\n";
	
	for (int i = 0; i < dependencies.size(); i++)
    {
	    output << "\tinitializer_" << i << "();\n";
    }
	
	output << "\tlib_handle = _elfcalls->dlopen_fatal(__elfname);\n"
		"}\n\n";
	
	for (int i = 0; i < dependencies.size(); i++)
    {
	    output << "static void* lib_" << i << "_handle;\n"
		    "static void destructor_" << i << "() {\n"
		    "\t_elfcalls->dlclose_fatal(lib_" << i << "_handle);\n"
		    "}\n\n";
    }

	output << "__attribute__((destructor)) static void destructor() {\n"
		"\t_elfcalls->dlclose_fatal(lib_handle);\n";
	
	for (int i = 0; i < dependencies.size(); i++)
    {
	    output << "\tdestructor_" << i << "();\n";
    }
	
	output << "}\n\n";
	
	for (const std::string& sym : symbols)
	{
		output << "void* " << sym << "() {\n"
			"\t__asm__(\".symbol_resolver _" << sym << "\");\n"
			"\treturn _elfcalls->dlsym_fatal(lib_handle, \"" << sym << "\");\n"
			"}\n\n";
	}

	output << "asm(\".section __TEXT,__elfname\\n"
		".private_extern ___elfname\\n"
		"___elfname: .asciz \\\"" << soname << "\\\"\");\n";
	
	for (int i = 0; i < dependencies.size(); i++)
    {
	    output << "asm(\".section __TEXT,__elfname_" << i << "\\n"
		    ".private_extern ___elfname_" << i << "\\n"
		    "___elfname_" << i << ": .asciz \\\"" << dependencies[i] << "\\\"\");\n";
    }
}

