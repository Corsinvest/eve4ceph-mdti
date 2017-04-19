/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1023

void main(int argc, char **argv) {

  FILE *fdiff;
  FILE *fimg;

  if (argc == 2 && strcmp(argv[1], "version") == 0) {
    //&& argv[1] == 'version'
    //   printf(argv[1]);
    printf("0.0.1\n");
    exit(0);
  }

  if (argc < 3) {
    printf("    ______      __                       _              _    "
           "________\n");
    printf("   / ____/___  / /____  _________  _____(_)_______     | |  / / "
           "____/\n");
    printf("  / __/ / __ \\/ __/ _ \\/ ___/ __ \\/ ___/ / ___/ _ \\    | | / / "
           "__/\n");
    printf(
        " / /___/ / / / /_/  __/ /  / /_/ / /  / (__  )  __/    | |/ / /___\n");
    printf("/_____/_/ /_/\\__/\\___/_/  / .___/_/  /_/____/\\___/     "
           "|___/_____/\n");
    printf("                         /_/\n\n");

    printf("EnterpriseVE Merge Diff file To Image for Ceph    (Made in "
           "Italy)\n\n");

    printf("Usage:\n");
    printf("    eve4ceph-mdti version\n");
    printf("    eve4ceph-mdti <FULL_IMAGE_FILE> <DIFF_FILE>\n\n");
    printf("Report bugs to <support@enterpriseve.com>.\n");
    exit(0);
  }

  // open image
  char *img = argv[1];
  fimg = fopen(img, "r+");
  if (fimg == NULL) {
    printf("Error opening image file %s\n", img);
    exit(1);
  }

  // opem diff file
  char *diff = argv[2];
  fdiff = fopen(diff, "r");
  if (fdiff == NULL) {
    printf("Error opening diff file %s\n", diff);
    exit(1);
  }

  // check header diff file
  char header[255];
  if (fread(header, 1, 12, fdiff) != 12) {
    printf("Error reading diff header\n");
    exit(1);
  }

  if (strncmp(header, "rbd diff v1", 11)) {
    printf("Unrecognised diff format\n");
    exit(1);
  }

  int debug = 0;

  // reading metadata
  guint8 tag;
  int reading_metadata = 1;

  printf("Reading metadata\n");
  while (reading_metadata) {
    if (fread(&tag, 1, 1, fdiff) != 1) {
      printf("Error reading metadata\n");
      exit(1);
    }

    guint32 name_len;
    //    int rlen;
    char name[MAX_LEN + 1];
    guint64 image_size;

    switch (tag) {
    case 'f': // from snap
    case 't': // to snaps
      fread(&name_len, 1, sizeof(guint32), fdiff);
      int rlen = name_len > MAX_LEN ? MAX_LEN : name_len;
      fread(name, 1, rlen, fdiff);
      name[rlen] = 0;
      printf("%s snap: %s\n", (tag == 'f' ? "From" : "To"), name);
      if (name_len > MAX_LEN) {
        fseek(fdiff, name_len - MAX_LEN, SEEK_CUR);
      }
      break;

    case 's': // size
      fread(&image_size, 1, sizeof(guint64), fdiff);
      printf("Image size: %lu (%luGB)\n", image_size,
             image_size / (1024 * 1024 * 1024));
      break;

    default:
      reading_metadata = 0;
      printf("End of metadata\n");
      fseek(fdiff, -1, SEEK_CUR);
      break;
    }
  }

  char *rbuf = (char *)malloc(4096);
  char *zbuf = (char *)malloc(4096);
  memset((void *)zbuf, 0, 4096);

  int total_length = 0;

  // reading data
  int reading_data = 1;
  while (reading_data) {
    if (fread(&tag, 1, 1, fdiff) != 1) {
      printf("Error reading data\n");
      exit(1);
    }

    guint64 offset;
    guint64 length;

    switch (tag) {
    case 'w':                                    // Update data
    case 'z':                                    // Zero data
      fread(&offset, 1, sizeof(guint64), fdiff); // offset
      fread(&length, 1, sizeof(guint64), fdiff); // length

      total_length += length;

      if (debug) {
        printf("Data at offset %lu of length %lu\n", offset, length);
      }

      int written = 0;
      while (written < length) {
        int l = (length - written > 4096) ? 4096 : length - written;

        if (tag == 'w') {
          if (fread(rbuf, 1, l, fdiff) != l) {
            printf("error reading diff file\n");
            exit(0);
          }
        }

        if (fseek(fimg, offset + written, SEEK_SET)) {
          printf("error seeking in image file\n");
          exit(0);
        }

        if (debug) {
          printf("writing %d bytes to image\n", l);
        }

        int n = 0;
        if (tag == 'w') {
          n = fwrite(rbuf, 1, l, fimg);
        } else if (tag == 'z') {
          n = fwrite(zbuf, 1, l, fimg);
        }

        written += n;
        if (n == 0) {
          printf("Error writing to image\n");
          exit(0);
        }
      }
      break;

    case 'e': // End data
      reading_data = 0;     
      printf("End of data\n");
      printf("Writing %d bytes to image\n", total_length);
      break;

    default:
      reading_data = 0;
      printf("Unknown data encountered %d\n", tag);
      break;
    }
  }

  fclose(fimg);
  fclose(fdiff);
}
