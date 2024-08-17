- Avoid using `scanf, fscanf`
- From the manual of `scanf`:
  > It is very difficult to use these functions correctly, and it is preferable to read entire lines with fgets(3) or getline(3) and parse them later with sscanf(3) or more  specialized functions such as strtol(3).
