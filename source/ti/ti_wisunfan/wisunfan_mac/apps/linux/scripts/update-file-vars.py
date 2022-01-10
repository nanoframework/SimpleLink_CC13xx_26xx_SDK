# 
import os
import fnmatch
import re

regoogle = re.compile("google")

rootdir   = '..'
# comment prefix is changed dynamically
cmtprefix = "????"

def load_file( filename ):
    with open(filename) as f:
        content = f.read().splitlines()
    return content

def matching_files( rootdir, pattern ):
    matches = []
    for root, dirnames, filenames in os.walk( rootdir ):
        # print "ROOT=%s\n" % root
        # print "FILENAMES=%s\n" % filenames
        for filename in fnmatch.filter(filenames,pattern):
            #print "MATCH %s\n" % filename
            matches.append(os.path.join(root,filename))
    return matches

def test_line( txt,findme ):
    # returns BOOL if found
    txt = txt.strip()
    # Look for common comment chars
    found = False
    #print "Find: %s, Line=%s\n" % (findme,txt)
    for c in [ '#', ';', '*' ]:
        if txt.startswith(c):
            found = True
            txt = txt.lstrip(c)
    # Ok now more complex versions
    if txt.startswith('//'):
        found = True
        txt = txt.lstrip('/')
    elif txt.startswith('/*'):
        # remove the / and any extra *s
        found = True
        txt = txt[1:].lstrip('*')
    #print "FOUND COMMENT: %s\n" % found
    if found:
        found = txt.strip().startswith(findme)
        #print "FOUND Text: %s\n" % found
    return found

def locate_line( content, sline,findthis ):
    # If we have already given up... 
    if sline < 0:
        # then do not continue to search
        return -1
    eline = len(content)
    #print "LOCATE: Start = %d to %d, find: %s\n" % (sline, eline, findthis )
    for x in range(sline,eline):
        # Does line begin with any type of comment char?
        if test_line( content[x], findthis ):
            # return line number
            return x
    # Not found
    return -1
        

def verify_var_present( filename ):
    #print "File: %s" % filename
    content = load_file(filename)
    # Be careful here, if you insert a nice explanitory comment
    # Emacs and VIM tend to *trigger* on *THIS* comment
    # The values in your nice explanitory comment are perhaps wrong
    # Emacs and VIM does not care... this makes life hard
    # 
    #  Texas Instruments Micro Controller Coding Style
    #  ===============================
    #  Local Variables:
    #  End:
    #  vim:set 

    # Where do we start (last 20 lines)
    sline = len(content)-20
    # Deal with short files!
    if sline < 0:
        sline = 0

    ti    = locate_line( content, sline, "Texas Instruments" )
    f_equal = locate_line( content, ti-1, "=======")
    e_qual = locate_line( content, ti   , "=======")
        
    # Start/End of local variable list
    svars = locate_line( content, e_qual, "Local Variables:")
    evars = locate_line( content, svars, "End:")
    vim   = locate_line( content, evars, "vim:set" )
    
    # Ensure everything is found and is in proper order
    #print "%d,%d,%d,%d,%d\n" % (ti,e_qual,svars,evars,vim)
    if (f_equal==(ti-1)) and (ti>2) and (e_qual==(ti+1)) and (svars>e_qual) and (evars>svars) and (vim>evars):
        # Success return location
        return [True,f_equal,vim]
    else:
        # Failure :-(
        return [False,0,0]

def write_backup( filename, content ):
    h = open( filename + '.bak','w')
    for l in content:
        # Python docs say: DO NOT USE 'os.linesep'
        # Instead, just use a normal '\n'
        h.write( l + '\n' )
    h.close()


def common_start( h ):
    h.write( "%s ========================================\n" % cmtprefix )
    h.write( "%s Texas Instruments Micro Controller Style\n" % cmtprefix )
    h.write( "%s ========================================\n" % cmtprefix )

def common_end( h ):
    # DO NOT WRITE any more stuff
    # otherwise ... the file slowly grows and grows
    pass

def emacs_start(h):
    h.write("%s Local Variables:\n" % cmtprefix )

def emacs_end(h):
    h.write("%s End:\n" % cmtprefix )

def emacs_var( h, name, value ):
    h.write( "%s %s: %s\n" % (cmtprefix, name, value) )

def vim_begin(h):
    h.write("%s vim:set " % cmtprefix )

def vim_end(h):
    h.write('\n')

def vim_var(h,name,value):
    h.write(" %s=%s" % (name, value))

def do_c_code( h, filename, content, sline, eline ):
    global cmtprefix
    cmtprefix = ' * '
    # h.write("/*\n")
    common_start(h)
    emacs_start(h)
    emacs_var( h, 'mode', 'c' )
    # this must actually be quoted
    emacs_var( h, 'c-file-style', '"bsd"' )
    emacs_var( h, 'tab-width', 4 )
    emacs_var( h, 'c-basic-offset', 4 )
    emacs_var( h, 'indent-tabs-mode', 'nil' )
    emacs_end(h)
    vim_begin(h)
    vim_var( h, 'filetype', 'c')
    vim_var( h, 'tabstop', 4 )
    vim_var( h, 'shiftwidth', 4 )
    vim_var( h, 'expandtab', 'true' )
    vim_end(h)
    common_end(h)
    # h.write(" */\n")

def do_makefile( h, filename, content, sline, eline ):
    global cmtprefix
    cmtprefix = '# '
    common_start(h )
    emacs_start(h)
    emacs_var( h, 'mode', 'makefile-gmake' )
    emacs_end(h)
    vim_begin(h)
    vim_var( h, 'filetype', 'make')
    vim_end(h)
    common_end(h)

def do_python( h, filename, content, sline, eline ):
    global cmtprefix
    cmtprefix = '# '
    common_start(h)
    emacs_start(h)
    emacs_var( h, 'mode', 'python' )
    emacs_end(h)
    vim_begin(h)
    vim_var( h, 'filetype', 'python')
    vim_end(h)
    common_end(h)

def do_shell( h, filename, content, sline, eline ):
    global cmtprefix
    cmtprefix = '# '
    common_start(h)
    emacs_start(h)
    emacs_var( h, 'mode', 'sh' )
    emacs_end(h)
    vim_begin(h)
    vim_var( h, 'filetype', 'sh')
    vim_end(h)
    common_end(h)


def rewrite_vars( h, filename, content, sline, eline ):
    # Remove the directory component
    # Keeping only the filename and extension
    fname = os.path.split(filename)[-1]

    # Try our different types
    if fnmatch.fnmatch( fname, '*.[ch]'):
        do_c_code( h, filename, content, sline, eline )
    elif fnmatch.fnmatch(fname, '*.mak' ):
        do_makefile( h, filename, content, sline, eline )
    elif fnmatch.fnmatch(fname, '[Mm]akefile' ):
        do_makefile( h, fname, content, sline, eline )
    elif fnmatch.fnmatch(filename, '*.py' ):
        do_python( h, fname, content, sline, eline )
    elif fnmatch.fnmatch(fname, '*.sh' ):
        do_shell( h, filename, content, sline, eline )
    else:
        raise Exception("No handler: %s" % filename )


def rewrite_file( filename, sline, eline ):
    if regoogle.match( filename ):
        print "Skip %s" % filename
        return

    print "Processing: %s" % filename
    content = load_file(filename)
    write_backup( filename, content )

    if (sline <= 0) or (eline <=0):
        sline = len(content)
        eline = len(content)

    h = open( filename, 'w' )
    for l in range(0,sline):
        txt = content[l]
        h.write( txt + '\n' )

    if (sline==eline):
        # Special case, there is no comment
        # Add a blank line before and after
        h.write('\n')
    rewrite_vars(h,filename, content, sline, eline )
    if (sline==eline):
        # Special case, there is no comment
        # Add a blank line before and after
        h.write('\n')
    
    for l in range( eline+1, len(content) ):
        txt = content[l]
        h.write( txt + '\n' )

    h.close
        

def get_file_list(r):
    l = []
    l = l + matching_files( r, '*.[ch]' )
    l = l + matching_files( r, '*.py' )
    l = l + matching_files( r, '*.sh' )
    l = l + matching_files( r, '*.mak' )
    l = l + matching_files( r, '[Mm]akefile' )
    l = l + matching_files( r, '[Gg][Nn][Uu]akefile' )
    return l


def update_filename( fname ):
    [ f,s,e ] = verify_var_present( fname )
    rewrite_file( fname, s, e )

def main():
    all_files = get_file_list(rootdir)
    for fname in all_files:
        update_filename( fname )


main()

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: python
#  End:
#  vim:set  filetype=python

