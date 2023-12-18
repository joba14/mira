
# !/bin/sh

# Check if a directory is provided
if [ -z "$1" ]; then
	echo "usage: $0 <directory>"
	exit 1
fi

directory="$1"

# Check if the directory exists
if [ ! -d "$directory" ]; then
	echo "[error]: provided directory '$directory' was not found."
	exit 1
fi

# Change into the provided directory
cd "$directory" || exit 1

# Find all .sh files in the current directory
sh_files=$(find . -type f -name "*.sh")

# Check if there are any .sh files
if [ -z "$sh_files" ]; then
	echo "[error]: no '*.sh' files found in the directory '$directory'."
	exit 0
fi

# Run each .sh file one by one
for file in $sh_files; do
	echo "[info]: running script '$file'."

	bash "$file"
	exit_code=$?

	if [ $exit_code -eq 0 ]; then
		echo "[info]: script '$file' completed successfully."
	else
		echo "[error]: script '$file' failed with exit code $exit_code."
		exit $exit_code
	fi
done
