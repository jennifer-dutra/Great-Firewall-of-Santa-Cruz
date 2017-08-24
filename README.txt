COMPILE
Make all

TO RUN THE PROGRAM

Reading in a file:

ex. ./newspeak <file_name.txt

Typing text into terminal:

ex. ./newspeak
    [Enter text when prompted. Press enter. Ctrl D.]

Any combination of flags is supported:
ex. ./newspeak -s - m -h 1000 -f 10000 <Great_Expectations.txt

-s statistics, -m move-to-front rule on, -b move-to-front rule off
-f bloom filter size, -h hash table size
