# generate checksums from c file
while read -r line; do printf %s "$line" | md5sum | cut -f1 -d' '; done < main.c > checksums.txt
