/* ls.c
  purpose list contents of directory or directories
  action if no args, use . else list files in args
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

struct File {
	char mode[11];
	int st_nlink;
	short st_nlink_length;
	ino_t inode_number;
	char* owner_name;
	short owner_name_length;
	char* group_name;
	short group_name_length;
	long st_size;
	blkcnt_t st_blocks;
	short st_size_length;
	char* time_of_last_modification;
	char* filename;
};

void show_file_info( char* , struct stat* );
void mode_to_letters( int , char[] );
char* uid_to_name( uid_t );
char* gid_to_name( gid_t );
struct File dostat( char *filename );

typedef struct {
	struct File *array;
	size_t used;
	size_t size;
	int block_total;
} Files;
void init_files(Files*, size_t initialSize);
void insert_file(Files*, struct File);
void free_files(Files*); 
int comp(const void*, const void*);
void sort(Files*); 
void parse(char[]);

struct File from_stat_to_user_standard_file(char *filename, struct stat *info_p) {
	char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
	struct File file = {
		.st_nlink=(int) info_p->st_nlink,
		.owner_name=uid_to_name(info_p->st_uid),
		.group_name=gid_to_name(info_p->st_gid),
		.inode_number= info_p->st_ino,
		.st_size=(int) info_p->st_size,
		.time_of_last_modification=4+ctime(&info_p->st_mtime),
		.filename=filename
	};
	mode_to_letters(info_p->st_mode, file.mode);
	file.owner_name_length = strlen(file.owner_name);
	file.group_name_length = strlen(file.group_name);
	file.st_blocks = info_p->st_blocks/2;
	file.st_nlink_length = floor(log10(file.st_nlink))+1;
	file.st_size_length = floor(log10(file.st_size_length))+1;
	return file;
}

void print_long_format(Files* files) {
	for(int i=0; i<files->used; i++) {
		printf( "%7lu " , files->array[i].inode_number );
		printf( "%14s ", files->array[i].filename);
		if (i%4 == 3) {
			printf("\n");
		}
	}
}

Files do_ls( char dirname[] ) {
	DIR *dir_ptr;
	struct dirent *direntp;
	if ( ( dir_ptr = opendir( dirname ) ) == NULL ) {
		fprintf(stderr,"ls: cannot open %s\n", dirname);
		exit(EXIT_FAILURE);
	}
	Files files;
	init_files(&files, 5);
	while ( (direntp = readdir(dir_ptr)) != NULL ) {
		if (strcmp(direntp->d_name,".")==0 || strcmp(direntp->d_name,"..")==0) 
			continue;
		insert_file(&files, dostat(direntp->d_name));
	}
	closedir(dir_ptr);	
	return files;
}


int main(int ac, char *av[]) {
	if ( ac == 1 ) {
		parse(".");
		return EXIT_SUCCESS;
	}
	parse(av[1]);
	return EXIT_SUCCESS;
}

/*
  Utility functions
*/

void parse(char dirname[]) {
	Files files = do_ls( dirname );
	sort(&files);
	print_long_format(&files);
	free_files(&files);
}

int compare_lexicographically(const void* a, const void* b) 
{
    return strcmp((*(struct File*) a).filename, (*(struct File*) b).filename);
}

void sort(Files *files) {
	qsort(files->array, files->used, sizeof(struct File), compare_lexicographically);
} 

void init_files(Files *a, size_t initialSize) {
	a->array = malloc(initialSize * sizeof(struct File));
	a->used = 0;
	a->size = initialSize;
	a->block_total = 0;
}

void free_files(Files *a) {
	free(a->array);
	a->array = NULL;
	a->used = a->size = a->block_total = 0;
}


void insert_file(Files *a, struct File file) {
	// a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
	// Therefore a->used can go up to a->size 
	if (a->used == a->size) {
		a->size *= 2;
		a->array = realloc(a->array, a->size * sizeof(struct File));
	}
	a->array[a->used++] = file;
	a->block_total += file.st_blocks;
}


/*

 * This function takes a mode value and a char array

 * and puts into the char array the file type and the

 * nine letters that correspond to the bits in mode.

 * NOTE: It does not code setuid, setgid, and sticky

 * codes

*/


void mode_to_letters( int mode, char str[] )

{

	strcpy( str, "----------" ); /* default=no perms */


	if ( S_ISDIR(mode) ) str[0] = 'd'; /* directory? */

	if ( S_ISCHR(mode) ) str[0] = 'c'; /* char devices */

	if ( S_ISBLK(mode) ) str[0] = 'b'; /* block device */


	if ( mode & S_IRUSR ) str[1] = 'r'; /* 3 bits for user */

	if ( mode & S_IWUSR ) str[2] = 'w';

	if ( mode & S_IXUSR ) str[3] = 'x';


	if ( mode & S_IRGRP ) str[4] = 'r'; /* 3 bits for group */

	if ( mode & S_IWGRP ) str[5] = 'w';

	if ( mode & S_IXGRP ) str[6] = 'x';


	if ( mode & S_IROTH ) str[7] = 'r'; /* 3 bits for other */

	if ( mode & S_IWOTH ) str[8] = 'w';

	if ( mode & S_IXOTH ) str[9] = 'x';

}


#include <pwd.h>


char *uid_to_name( uid_t uid )

	/*

	 * returns pointer to username associated with uid, uses getpw()

*/

{

	struct passwd *getpwuid(), *pw_ptr;

	static char numstr[10];


	if ( ( pw_ptr = getpwuid( uid ) ) == NULL ){

		sprintf(numstr,"%d", uid);

		return numstr;

	}

	else

		return pw_ptr->pw_name ;

}


#include <grp.h>


char *gid_to_name( gid_t gid )

	/*

	 * returns pointer to group number gid. used getgrgid(3)

*/

{

	struct group *getgrgid(), *grp_ptr;

	static char numstr[10];


	if ( ( grp_ptr = getgrgid(gid) ) == NULL ){

		sprintf(numstr,"%d", gid);

		return numstr;

	}

	else

		return grp_ptr->gr_name;

}

struct File dostat( char *filename ) {
	struct stat info;
	if ( stat(filename, &info) == -1 ) 
		perror( filename ); 
        return from_stat_to_user_standard_file(filename, &info);
}


