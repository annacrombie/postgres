s/<type 'exceptions\.\([[:alpha:]]*\)'>/<class '\1'>/g
s/<type 'long'>/<class 'int'>/g
s/\([0-9][0-9]*\)L/\1/g
s/\([ [{]\)u"/\1"/g
s/\([ [{]\)u'/\1'/g
s/def next/def __next__/g
s/LANGUAGE plpythonu/LANGUAGE plpython3u/g
s/LANGUAGE plpython2u/LANGUAGE plpython3u/g
s/EXTENSION plpythonu/EXTENSION plpython3u/g
s/EXTENSION plpython2u/EXTENSION plpython3u/g
s/EXTENSION \([^ ]*\)_plpythonu/EXTENSION \1_plpython3u/g
s/EXTENSION \([^ ]*\)_plpython2u/EXTENSION \1_plpython3u/g
s/installing required extension "plpython2u"/installing required extension "plpython3u"/g
