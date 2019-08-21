# edr2hpp

Creates a c++ header from an input file as an unsigned char array which can be used as an embedded resource in another binary. The unsigned char array will be wrapped in a namespace. Also in the namespace will be the name of the input file used to generate it and the length of the array data as an unsigned int.

## Usage:

```
rec_em INPUT [OPTIONS]
```

- INPUT

  an be a single file or a directory containing multiple files. If a directory is given all files within the directory will be added to the generated header.

- OPTIONS

  -o, --option
  NAME : Output filename for generated header. The default is 'rec_em'.
  
  -d, --directory
  PATH : Path to write the generated header. Path will be recursively
  created if it doesn't exist.
  
  -n, --namespace
  NAME : Namespace to be used in the generated header.
  
  -h, --help
  : Prints this help message.
  
  -v, --version
  : Print version.

## Example 1:

```
rec_em File1.txt -o file1_txt -d embedded_include -n embedded_file
```

- This would create the header file 'file1_txt.hpp' in the directory 'embedded_include' with the unsigned char array inside the namespace `embedded_file{}`.

## Example 2:

```
rec_em ../input_directory -o all_txt_files -d emb_include -n embedded_file
```

- Assuming the directory `../input_directory` contains three files (`a.txt, b.txt, c.txt`), this would create a single file `all_txt_files.hpp` in the `emb_include` directory in the `embedded_file{}` namespace. Each file's char array will be uniquely named based on the input filenames.
