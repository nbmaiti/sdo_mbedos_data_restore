/* 
 *File to restore data folder in mbedos build
 *
 * Author: Nabendu Maiti (nbmaiti@gmail.com)
 */

#include "FATFileSystem.h"
#include "mbed.h"
#include <errno.h>
#include <stdio.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"


#define SD_MOUNT_POINT "sd"
SDBlockDevice bd(PE_6, PE_5, PE_2, PE_4);
FATFileSystem fs(SD_MOUNT_POINT, &bd);

int File_Copy(char FileSource[], char FileDestination[]) {
  int c;
  FILE *stream_R;
  FILE *stream_W;

  stream_R = fopen(FileSource, "r");
  if (stream_R == NULL)
    return -1;
  stream_W = fopen(FileDestination, "w+"); // create and write to file
  if (stream_W == NULL) {
    fclose(stream_R);
    return -2;
  }
  while ((c = fgetc(stream_R)) != EOF)
    fputc(c, stream_W);
  fclose(stream_R);
  fclose(stream_W);

  return 0;
}

void return_error(int ret_val) {
  if (ret_val)
    printf("Failure. %d\r\n", ret_val);
  else
    printf("done.\r\n");
}

void errno_error(void *ret_val) {
  if (ret_val == NULL)
    printf(" Failure. %d \r\n", errno);
  else
    printf(" done.\r\n");
}

int main() {
  int error = 0;
  printf("Restoring blob and secret datas.\r\n");

  printf("Mounting the filesystem on \"/sd\". ");
  error = fs.mount(&bd);
  return_error(error);

  printf("Normal.blob :\n");
  if (File_Copy("/sd/ecdsa/Normal.blob", "/sd/data/Normal.blob") != 0)
    return -1;

  printf("Secure.blob :\n");
  if (File_Copy("/sd/ecdsa/Secure.blob", "/sd/data/Secure.blob") != 0)
    return -1;

  printf("platform_hmac_key.bin :\n");
  if (File_Copy("/sd/ecdsa/platform_hmac_key.bin",
                "/sd/data/platform_hmac_key.bin") != 0)
    return -1;

  printf("platform_aes_key.bin :\n");
  if (File_Copy("/sd/ecdsa/platform_aes_key.bin",
                "/sd/data/platform_aes_key.bin") != 0)
    return -1;

  printf("platform_iv.bin :\n");
  if (File_Copy("/sd/ecdsa/platform_iv.bin", "/sd/data/platform_iv.bin") != 0)
    return -1;

  printf("manufacturer_ip.bin :\n");
  if (File_Copy("/sd/ecdsa/manufacturer_ip.bin",
                "/sd/data/manufacturer_ip.bin") != 0)
    return -1;

  printf("mfg_proxy.dat :\n");
  if (File_Copy("/sd/ecdsa/mfg_proxy.dat", "/sd/data/mfg_proxy.dat") != 0)
    return -1;

  printf("rv_proxy.dat :\n");
  if (File_Copy("/sd/ecdsa/rv_proxy.dat", "/sd/data/rv_proxy.dat") != 0)
    return -1;

  printf("owner_proxy.dat :\n");
  if (File_Copy("/sd/ecdsa/owner_proxy.dat", "/sd/data/owner_proxy.dat") != 0)
    return -1;

  printf(" done.\r\n");

  printf("Opening root directory.");
  DIR* dir = opendir("/sd/data/");
  errno_error(dir);

  struct dirent* de;
  printf("Printing all filenames:\r\n");
  while((de = readdir(dir)) != NULL){
    printf("  %s\r\n", &(de->d_name)[0]);
  }

  printf("Closing root directory. ");
  error = closedir(dir);
  return_error(error);

  printf("File restore complete.\r\n");

  while (true) {
  }
}
