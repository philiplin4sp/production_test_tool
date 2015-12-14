from xml.dom.minidom import parse
import re
import sys
import datetime
try:
    from hashlib import sha256 as hash
except:
    from md5 import new as hash
import copy

defaultWidth = 80

SYMBOL_LUT = {}
TYPE_LUT   = {}


#*******************************************************************************
# Error classes
#*******************************************************************************

# tag error
#  raised when a tag is not found in a node
class TagError(Exception):
    def __init__(self, tag, node):
        self.tagText = "Missing key: %s\nin node: \n%s" %(tag, node.toxml())

    def __call__(self):
        return self.tagText

    def __str__(self):
        return repr(self.tagText)

class InstanceError(Exception):
    def __init__(self, name, inst):
        self.tagText = "Instance entry but no parent in name: %s, inst: %s" %(name, inst)

    def __call__(self):
        return self.tagText

    def __str__(self):
        return repr(self.tagText)


#*******************************************************************************
# tag helper functions
#*******************************************************************************

# get an optional tag
# calls getTag and catches the exception if the tag doesn't exist
def getOptionalTag(node, tag):
    try:
        return getTag(node, tag)
    except TagError:
        return ""

# get a tag
# find an element with the supplied tag, raises an exception if not present
# only returns elements that are direct children of the supplied node
def getTag(node, tag):
    el = node.getElementsByTagName(tag)
    if el.length == 0:
        raise TagError(tag, node)
    # now iterate over the results looking for something a direct child of the node
    for e in el:
        if e.parentNode == node:
            # got something, does it have children
            if el[0].hasChildNodes():
                return el[0].childNodes[0].data
            else:
                return ""
    # there are no direct children, complain
    return ""
    raise TagError(tag, node)


#*******************************************************************************
# symbol classes
#*******************************************************************************

# comment class
# used to contain a comment, does not support the standard set of APIs, only
# supports the buildComment method
class comment:
    def __init__(self, node):
        self.comment = getOptionalTag(node, "comment")
        if self.comment == "":
            self.comment = []
        else:
            self.comment = self.comment.replace("\n"," ")
            while self.comment.find("  ") != -1:
                self.comment = self.comment.replace("  ", " ")
            self.comment = self.comment.split(" ")

    def buildKalComment(self, indent="", width=defaultWidth):
        return self.__buildComment(indent, width, "/*", "*/")

    def buildCComment(self, indent="", width=defaultWidth):
        return self.__buildComment(indent, width, "/*", "*/")

    def buildMatlabComment(self, indent="", width=defaultWidth):
        return self.__buildComment(indent, width, "%", "")

    def buildPythonComment(self, indent="", width=defaultWidth):
        return self.__buildComment(indent, width, "#", "")

    def __buildComment(self, indent, width, comStart, comEnd):
        if len(self.comment) == 0:
            return ""
        # if no width is supplied just print the line
        if width == None:
            return indent+comStart+" "+self.comment+" "+comEnd
        # we need to do something a little more clever
        dec = ""
        line = indent+comStart
        for c in self.comment:
            # work out if this will push us over the limit, remember 1 space
            if (len(line)+len(c)+len(comEnd)+1) > width:
                line = ("%%-%ds%%s" % (width-len(comEnd))) % (line, comEnd)
                dec += line.rstrip()+"\n"
                line = indent+comStart
            line += " "+c
        line = ("%%-%ds%%s" % (width-len(comEnd))) % (line, comEnd)
        dec += line.rstrip()+"\n"
        return dec

# base class everyone inherits from
class symbol:
    def __init__(self, node, type):
        self.name    = getTag(node, "name")
        self.info    = getTag(node, "info")
        self.comment = comment(node)
        self.type    = type
        m = hash()
        m.update(self.name)
        m.update(self.info)
        m.update(self.type)
        self.link    = "a"+m.hexdigest()

    def getType(self):
        return self.type

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getLink(self):
        return self.link

    def getSize(self):
        return 1

    def buildKalDeclaration(self):
        raise Exception, "Unimplemented function in symbol: "+self.name

    def buildCDeclaration(self):
        raise Exception, "Unimplemented function in symbol: "+self.name

    def buildMatlabDeclaration(self):
        raise Exception, "Unimplemented function in symbol: "+self.name

    def buildPythonDeclaration(self):
        raise Exception, "Unimplemented function in symbol: "+self.name

    def buildSimpleInfoDeclaration(self):
        if self.info == "":
            raise Exception, "Info block empty in symbol: "+self.name
        # buid some html to show
        dec  = "  <tr>\n"
        dec += "    <td><a href=\"#"+self.link+"\">"+self.name+"</a></td>\n"
        dec += "    <td>"+self.getType()
        if self.type == "typedef":
            dec += "("+self.define.getType()+")"
        dec += "</td>\n"
        dec += "    <td>"+self.info+"</td>\n"
        dec += "  </tr>\n"
        return dec

    def buildFullInfoDeclaration(self, title=0):
        if title:
            dec = "<a class=\"anchor\" name=\""+self.link+"\"></a><h2><tt>"+self.name+"</tt></h2>"
            dec += "<p>"+self.info+"</p>"
        else:
            dec  = "<tr>\n  <td><b>"+self.name+"</b></td>\n"
            dec += "  <td>"+self.info+"</td>\n</tr>\n"
        if title:
            dec += "<hr>\n"
        return dec


class preproc:
    def __init__(self, node, type):
        # defines are all quite different so make everything optional
        self.name    = getOptionalTag(node, "name")
        self.value   = getOptionalTag(node, "value")
        self.test    = getOptionalTag(node, "test")
        self.comment = comment(node)
        # save the type
        self.type = type

    def getType(self):
        return self.type

    def getName(self):
        return self.name

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        return ""
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        return ""
    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        return ""
    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        return ""
    def buildSimpleInfoDeclaration(self):
        return ""
    def buildFullInfoDeclaration(self, title=0):
        return ""




# general class
# class for standard symbols, i.e. not structures or typedefs
class general(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "general")
        # get the stuff that must be present
        self.subtype = getTag(node, "subtype")
        # check if this is a symbol we already know of
        self.subsym = checkLookup(self.subtype)
        # is there a size
        self.size = getOptionalTag(node, "size")
        if self.size == "":
            self.size = "0"
        self.size = int(self.size)
        if self.size != 0:
            self.subsym = None

    def getSize(self):
        if self.subsym == None:
            if self.size == 0:
                return 1
            else:
                return self.size
        else:
            if self.size == 0:
                return self.subsym.getSize()
            else:
                return self.size * self.subsym.getSize()

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        if self.subsym == None:
            if len(parent) == 0:
                # is there anyting to do here?
                return ""
            # have we got an offset
            offset=0
            if kwargs.has_key("offset"):
                offset = kwargs["offset"]
            # change camel case to underscores
            name = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
            # put the field on the end
            name += "_FIELD"
            # print the comment
            dec = self.comment.buildKalComment(indent)
            # this is in a structure, delcare with the parent name
            if len(parent) != 0:
                # change camel case to underscores
                parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
                name = parent+"."+name
            dec += indent+".CONST $%-60s    %d;" % (name, offset)
            return dec
        else:
            return self.subsym.buildKalDeclaration(indent, parent+"."+self.getName(), **kwargs)

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment(indent)
        if self.size == 0:
            return dec+indent+"%s %s;" % (self.subtype, self.name)
        else:
            return dec+indent+"%s %s[%d];" % (self.subtype, self.name, self.size)

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            # is there anyting to do here?
            return ""
        # have we got an offset
        offset=0
        if kwargs.has_key("offset"):
            offset = kwargs["offset"]
        # change camel case to underscores
        name   = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
        # print the comment
        dec = self.comment.buildMatlabComment(indent)
        # this is in a structure, delcare with the parent name
        if len(parent) != 0:
            # change camel case to underscores
            parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
            fullname = parent+"."+name

        dec += indent+"%-60s = %d;\n" % (fullname, offset)
        dec += indent+"%-60s = %d;\n" % (parent+".rev{end+1, 1}", offset)
        dec += indent+"%-60s = '%s';" % (parent+".rev{end,   2}", name)
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            # is there anyting to do here?
            return ""
        # have we got an offset
        offset=0
        if kwargs.has_key("offset"):
            offset = kwargs["offset"]
        # change camel case to underscores
        name   = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
        # print the comment
        dec = self.comment.buildPythonComment(indent)
        # this is in a structure, delcare with the parent name
        # change camel case to underscores
        parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
        dec += "%s%s.addElement('%s', %d)"%(indent, parent, name, offset)
        return dec



# structure class
# class for structures
class structure(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "struct")
        self.inst = getOptionalTag(node, "instance")
        self.members = []
        for mem in node.getElementsByTagName("members"):
            if mem.parentNode != node:
                continue
            for sub in mem.getElementsByTagName("symbol"):
                if sub.parentNode != mem:
                    continue
                self.addMember(declareSymbol(sub))

    def addMember(self, member):
        self.members.append(member)


    def getSize(self):
        return sum(m.getSize() for m in self.members)

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        if parent == "":
            if self.inst != "":
                raise InstanceError(self.name, self.inst)
            typedefName = self.name
            if kwargs.has_key("typedefName"):
                typedefName = kwargs["typedefName"]
        else:
            typedefName = parent
            if self.inst != "":
                typedefName += "."+self.inst
            elif self.name != "":
                typedefName += "."+self.name
        # make it look pretty
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # initialise the output and the offset count
        dec  = indent+"// %s\n" % (typedefName.upper())
        dec += self.comment.buildKalComment(indent)
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildKalDeclaration(indent, typedefName, offset=off)+"\n"
            off += m.getSize()
        if parent == "":
            dec += "%s.CONST %-60s     %d;\n" % (indent, "$"+typedefName.lower()+".STRUC_SIZE", off)
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec  = self.comment.buildCComment(indent)
        name = " "+self.name
        name = name.rstrip()
        dec += "%sstruct%s\n%s{\n" % (indent, name, indent)
        for m in self.members:
            dec += m.buildCDeclaration(indent+"  ")+"\n"
        dec += indent+"};"
        if len(self.inst) != 0:
        # this is another horrible work around, but replace the last character
        # (a semi-colon) with a newline
            dec = dec[:-1]
            dec += " "+self.inst+";"
        return dec

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        typedefName = self.name
        if kwargs.has_key("typedefName"):
            typedefName = kwargs["typedefName"]
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # do we have a parent
        if len(parent) != 0:
            typedefName = parent+"."+typedefName
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # initialise the output and the offset count
        dec  = indent+"   %% EC.%s\n" % (typedefName.upper())
        dec += self.comment.buildMatlabComment("")
        dec += indent+"   EC.%-60s = {};\n" % (typedefName.lower()+".rev")
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildMatlabDeclaration("   EC.", typedefName, offset=off)+"\n"
            off += m.getSize()
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        typedefName = self.name
        if kwargs.has_key("typedefName"):
            typedefName = kwargs["typedefName"]
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # do we have a parent
        if len(parent) != 0:
            typedefName = parent+"."+typedefName
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # initialise the output and the offset count
        dec  = indent+"# %s\n" % (typedefName.upper())
        dec += self.comment.buildPythonComment(indent)
        dec += indent+"%s = encorePdu.Lookup()\n" % (typedefName.lower())
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildPythonDeclaration(indent, typedefName, offset=off)+"\n"
            off += m.getSize()
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec


    def buildTypedefInfoDeclaration(self):
        if len(self.name) == 0:
            dec  = "<p>Aliases an anonymous structure.</p>\n"
        else:
            dec  = "<p>Aliases a structure, called: '"+self.name+"'.</p>\n"
        dec += "<p>"+self.info+"</p>"
        dec += "<table border=1>\n"
        for m in self.members:
            dec += m.buildFullInfoDeclaration()
        dec += "</table>\n"
        return dec

    def buildFullInfoDeclaration(self, title=0):
        if title:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><h2><tt>struct "+self.name+"</tt></h2>"
        else:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><p>struct "+self.name+"</p>"
        dec += "<p>"+self.info+"</p>"
        dec += "<p>Contains the following members:</p>\n"
        dec += "<table border=1>\n"
        for m in self.members:
            dec += m.buildFullInfoDeclaration()
        dec += "</table>\n"
        if title:
            dec += "<hr>\n"
        return dec


# typedef class
# class to hold typedefs
class typedef(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "typedef")
        # say we don't alias a structure
        self.sym = None
        # now work out what the define is
        define = node.getElementsByTagName("define")
        if define.length == 0:
            raise TagError("define", node)
        define = define[0]
        syms   = define.getElementsByTagName("symbol")
        if syms.length == 0:
            # this is just a value
            self.define = value(define.childNodes[0].data)
        else:
            # this is the structure
            self.define = declareSymbol(syms[0])

    def setStruct(self, symbol):
        self.sym = symbol

    def getDefineValue(self):
        return self.define.getValue()

    def getSize(self):
        if self.sym != None:
            return self.sym.getSize()
        return self.define.getSize()

    def getAliasType(self):
        if self.sym != None:
            return self.sym.getType()
        return self.define.getType()

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        if self.sym != None:
            return dec+self.sym.buildKalDeclaration(indent, parent, typedefName=self.name, **kwargs)
        else:
            return dec+self.define.buildKalDeclaration(indent, parent, typedefName=self.name, **kwargs)

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec  = self.comment.buildCComment("")
        dec += "typedef "
        # if we're aliasing a struct put struct on the front
        if self.sym != None:
            if self.sym.getType() != "typedef":
                dec += self.sym.getType()+" "
        dec += self.define.buildCDeclaration()
        # this is another horrible work around, but replace the last character
        # (a semi-colon) with a space
        if self.define.getType() != "value":
            dec = dec[:-1]
        dec += " "+self.name+";"
        return dec

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildMatlabComment(indent)
        if self.sym != None:
            return dec+self.sym.buildMatlabDeclaration(indent, "", typedefName=self.name)
        else:
            return dec+self.define.buildMatlabDeclaration(indent, "", typedefName=self.name)

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildPythonComment(indent)
        if self.sym != None:
            return dec+self.sym.buildPythonDeclaration(indent, "", typedefName=self.name)
        else:
            return dec+self.define.buildPythonDeclaration(indent, "", typedefName=self.name)


    def buildFullInfoDeclaration(self, title):
        # if this is the title make it look like it
        if title:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><h2><tt>typedef "+self.name+"</tt></h2>"
        else:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><p>typedef "+self.name+"</p>"
        # iclude any information about this field
        dec += "<p>"+self.info+"</p>"
        # if this aliases another symbol we know about, just link to that
        if self.sym != None:
            dec += "<p>Aliases: <a href=\"#"+self.sym.getLink()+"\">"+self.sym.getName()+"</a></p>"
        else:
            # this may be an anonymous symbol, in which case print that out properly
            if self.define.getType() == "value":
                dec += "<p>Aliases: "+self.define.getValue()+"</p>"
            elif self.define.getType() == "struct":
                dec += self.define.buildTypedefInfoDeclaration()
            elif self.define.getType() == "enum":
                dec += self.define.buildTypedefInfoDeclaration()
            else:
                dec += "<p>Aliases: "+self.define.getType()+" "+self.define.getName()+"</p>"
        if title:
            dec += "<hr>\n"
        return dec

# union class
# holds union constructs
class union(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "union")
        self.inst = getOptionalTag(node, "instance")
        # work through the list of members
        self.members = []
        for mem in node.getElementsByTagName("members"):
            if mem.parentNode != node:
                continue
            for sub in mem.getElementsByTagName("symbol"):
                if sub.parentNode != mem:
                    continue
                self.addMember(declareSymbol(sub))

    def addMember(self, member):
        self.members.append(member)

    def getSize(self):
        return reduce(max, [m.getSize() for m in self.members])

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        if parent == "":
            if self.inst != "":
                raise InstanceError(self.name, self.inst)
            typedefName = self.name
            if kwargs.has_key("typedefName"):
                typedefName = kwargs["typedefName"]
        else:
            typedefName = parent
            if self.inst != "":
                typedefName += "."+self.inst
            elif self.name != "":
                typedefName += "."+self.name
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # initialise the output and the offset count
        dec  = indent+"// %s\n" % (typedefName.upper())
        dec += self.comment.buildKalComment(indent+"   ")
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildKalDeclaration(indent, typedefName, offset=off)+"\n"
        if parent == "":
            dec += "%s.CONST %-60s     %d;\n" % (indent, "$"+typedefName.lower()+".STRUC_SIZE", self.getSize())
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec  = self.comment.buildCComment(indent)
        name = " "+self.name
        name = name.rstrip()
        dec += "%sunion%s\n%s{\n" % (indent, name, indent)
        for m in self.members:
            dec += m.buildCDeclaration(indent+"  ")+"\n"
        dec += indent+"};"
        if len(self.inst) != 0:
        # this is another horrible work around, but replace the last character
        # (a semi-colon) with a newline
            dec = dec[:-1]
            dec += " "+self.inst+";"
        return dec

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        typedefName = self.name
        if kwargs.has_key("typedefName"):
            typedefName = kwargs["typedefName"]
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # do we have a parent
        if len(parent) != 0:
            typedefName = parent+"."+typedefName
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # initialise the output and the offset count
        dec  = indent+"   %% EC.%s\n" % (typedefName.upper())
        dec += self.comment.buildMatlabComment("")
        dec += indent+"   EC.%-60s = {};\n" % (typedefName.lower()+".rev")
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildMatlabDeclaration("   EC.", typedefName, offset=off)+"\n"
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        # should we be using the typedef name of the structure's name
        typedefName = self.name
        if kwargs.has_key("typedefName"):
            typedefName = kwargs["typedefName"]
        # have we been given an offset
        off = 0
        if kwargs.has_key("offset"):
            off = kwargs["offset"]
        # do we have a parent
        if len(parent) != 0:
            typedefName = parent+"."+typedefName
        typedefName = re.sub("([a-z])([A-Z])", r"\1_\2",typedefName)
        # initialise the output and the offset count
        dec  = indent+"# %s\n" % (typedefName.upper())
        dec += self.comment.buildPythonComment(indent)
        dec += indent+"%s = encorePdu.Lookup()\n" % (typedefName.lower())
        for m in self.members:
            # ask the member to make it's output
            dec += m.buildPythonDeclaration(indent, typedefName, offset=off)+"\n"
            off += m.getSize()
        # again this is a bit naf, but strip off the last new line
        dec = dec[:-1]
        return dec

    def buildTypedefInfoDeclaration(self):
        if len(self.name) == 0:
            dec  = "<p>Aliases an anonymous union.</p>\n"
        else:
            dec  = "<p>Aliases a union, called: '"+self.name+"'.</p>\n"
        dec += "<p>"+self.info+"</p>"
        dec += "<table border=1>\n"
        for m in self.members:
            dec += m.buildFullInfoDeclaration()
        dec += "</table>\n"
        return dec

    def buildFullInfoDeclaration(self, title=0):
        if title:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><h2><tt>union "+self.name+"</tt></h2>"
        else:
            dec  = "<a class=\"anchor\" name=\""+self.link+"\"></a><p>union "+self.name+"</p>"
        dec += "<p>"+self.info+"</p>"
        dec += "<p>Contains the following members:</p>\n"
        dec += "<table border=1>\n"
        for m in self.members:
            dec += m.buildFullInfoDeclaration()
        dec += "</table>\n"
        if title:
            dec += "<hr>\n"
        return dec




# define class
# holds #defines
class define(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "define")

    def setValue(self, value):
        self.value = value

    def getValue(self):
        return self.value

    def getSize(self):
        return 0

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment(indent)
        # XAP compiler doesn't like pre-processor commands not left justified so
        # ignore indent
        return dec+self.__buildDeclaration("#", "")

    def __buildDeclaration(self, pre, indent):
        if len(self.value) == 0:
            return indent+"%sdefine %s" % (pre, self.name)
        else:
            return indent+"%sdefine %s %s" % (pre, self.name, self.value)


# enum entry class
# holds enum entries, shuold only occur within enum lists
class enumEntry(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "enumEntry")
        self.value   = getTag(node, "value")
        # if there was a value then convert to a number
        if len(self.value) == 0:
            self.value = None
        else:
            if self.value[0:2] == "0x":
                self.value = int(self.value, 16)
            else:
                try:
                    self.value = int(self.value)
                except:
                    pass

    def getValue(self):
        return self.value

    def buildKalDeclaration(self, indent="", parent="", val_req="", **kwargs):
        if not kwargs.has_key("offset"):
            raise Exception, "Enum entry display called without offset"
        offset = kwargs["offset"]
        # does this one reset the offset
        if self.value != None:
            offset = self.value
        else:
            if val_req == "yes":
                raise Exception, "Enum entry value missing for: "+parent+"."+self.name 
        # change camel case to underscores
        parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
        name   = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
        name = parent+"."+name
        # comment?
        dec = self.comment.buildKalComment(indent)
        # print out
        return dec+indent+".CONST $%-60s    %d;" %(name, offset)

    def buildCDeclaration(self, indent="", parent="", val_req="", **kwargs):
        dec = self.comment.buildCComment(indent)
        name = parent.upper()+"_"+self.name
        if self.value == None:
            if val_req == "yes":
                raise Exception, "Enum entry value missing for: "+parent+"."+self.name
            else:
                return dec+indent+name
        else:
            return dec+indent+name+" = "+str(self.value)

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        if not kwargs.has_key("offset"):
            raise Exception, "Enum entry display called without offset"
        offset = kwargs["offset"]
        # does this one reset the offset
        if len(self.value) != 0:
            # it does use its value
            offset = int(self.value)
        # change camel case to underscores
        parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
        name   = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
        fullname = parent+"."+name
        # comment?
        dec = self.comment.buildMatlabComment(indent)
        # print out
        dec += indent+"%-60s = %d;\n" % (fullname, offset)
        dec += indent+"%-60s = %d;\n" % (parent+".rev{end+1, 1}", offset)
        dec += indent+"%-60s = '%s';;" % (parent+".rev{end,   2}", name)
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        if not kwargs.has_key("offset"):
            raise Exception, "Enum entry display called without offset"
        offset = kwargs["offset"]
        # does this one reset the offset
        if len(self.value) != 0:
            # it does use its value
            offset = int(self.value)
        # change camel case to underscores
        parent = re.sub("([a-z])([A-Z])", r"\1_\2",parent).lower()
        name   = re.sub("([a-z])([A-Z])", r"\1_\2",self.name).upper()
        # comment?
        dec = self.comment.buildPythonComment(indent)
        # print out
        dec += "%s%s.addElement('%s', %d)"%(indent, parent, name, offset)
        return dec


class enum(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "enum")
        self.val_req   = getOptionalTag(node, "values_required")
        self.entries = []
        for entry in node.getElementsByTagName("symbol"):
            ent = declareSymbol( entry )
            if ent.getType() != "enumEntry":
                raise Exception, "Incorrect entry '"+ent.getType()+"' found in enumeration:\n"+node.toxml()
            self.entries.append(ent)

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if kwargs.has_key("typedefName"):
            name = kwargs["typedefName"]
        off  = 0
        if len(parent) != 0:
            name = parent+"."+self.name
        dec  = indent+"   // %s\n" % (name)
        dec += self.comment.buildKalComment(indent)
        for e in self.entries:
            dec += e.buildKalDeclaration(indent, name, self.val_req, offset=off)+"\n"
            # does this reset the value, either way increment after
            if e.getValue() != None:
                off = e.getValue()
            off += 1
        return dec

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec   = self.comment.buildCComment("")
        dec  += "enum %s\n{\n" % (self.name)
        for e in self.entries:
            dec += e.buildCDeclaration("  ",self.name, self.val_req)+",\n"
        # this is a bit horrible but drop the last comma
        dec = dec[:-2]+"\n"
        # finish off
        dec += "};"
        return dec

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if kwargs.has_key("typedefName"):
            name = kwargs["typedefName"]
        if len(parent) != 0:
            name = parent+"."+self.name
        name = re.sub("([a-z])([A-Z])", r"\1_\2",name).lower()
        dec  = indent+"   %% %s\n" % (name)
        dec += self.comment.buildMatlabComment(indent+"   ")
        dec += indent+"   EC.%-60s = {};\n" % (name+".rev")
        off  = 0
        for e in self.entries:
            dec += e.buildMatlabDeclaration(indent+"   EC.", name, offset=off)+"\n"
            # does this reset the value, either way increment after
            if e.getValue() != None:
                off = e.getValue()
            off += 1
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if kwargs.has_key("typedefName"):
            name = kwargs["typedefName"]
        if len(parent) != 0:
            name = parent+"."+self.name
        name = re.sub("([a-z])([A-Z])", r"\1_\2",name).lower()
        dec  = indent+"# %s\n" % (name)
        dec += self.comment.buildPythonComment(indent)
        dec += indent+"%s = encorePdu.Lookup()\n" % (name)
        off  = 0
        for e in self.entries:
            dec += e.buildPythonDeclaration(indent, name, offset=off)+"\n"
            # does this reset the value, either way increment after
            if e.getValue() != None:
                off = e.getValue()
            off += 1
        return dec


    def buildTypedefInfoDeclaration(self):
        if len(self.name) == 0:
            dec  = "<p>Aliases an anonymous enumeration.</p>\n"
        else:
            dec  = "<p>Aliases a enumeration, called: '"+self.name+"'.</p>\n"
        dec += "<p>"+self.info+"</p>"
        dec += "<table border=1>\n"
        for e in self.entries:
            dec += e.buildFullInfoDeclaration()
        dec += "</table>\n"
        return dec





class listEntry(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "listEntry")
        self.value = getTag(node, "value")
        # if there was a value then convert to a number
        if len(self.value) == 0:
            self.value = None
        else:
            if self.value[0:2] == "0x":
                self.value = int(self.value, 16)
            else:
                try:
                    self.value = int(self.value)
                except:
                    pass

    def setValue(self, value):
        self.value = value

    def getValue(self):
        return self.value

    def getSize(self):
        return 0

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        name = self.name
        if len(parent) != 0:
            name = parent+"."+self.name
        return dec+indent+".CONST %-60s 0x%04X;" % ("$"+name, self.value)

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment("")
        name = self.name
        if len(parent) != 0:
            name = parent.upper()+"_"+self.name
            # XAP compiler doesn't like pre-processor commands not left justified so
        # ignore indent
        return dec+"#define %-60s 0x%04X" % (name, self.value)

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            parent = "list"
        fullname = parent+"."+self.name
        dec  = self.comment.buildMatlabComment(indent)
        dec += indent+"%-60s = %d;\n" % (fullname, self.value)
        dec += indent+"%-60s = %d;\n" % (parent+".rev{end+1,1}", self.value)
        dec += indent+"%-60s = '%s';" % (parent+".rev{end,  2}", self.name)
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            parent = "list"
        dec  = self.comment.buildPythonComment(indent)
        dec += "%s%s.addElement('%s', %d)"%(indent, parent, self.name, self.value)
        return dec



class listNumEls(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "listNumEls")

    def setValue(self, value):
        self.value = value

    def getValue(self):
        return self.value

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        name = self.name
        if len(parent) != 0:
            name = parent+"."+self.name
        return dec+indent+".define %-60s 0x%04X" % ("$"+name, self.value)

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment()
        name = self.name
        if len(parent) != 0:
            name = parent.upper()+"_"+self.name
        return dec+"#define %-60s 0x%04X" % (name, self.value)

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            parent = "list"
        fullname = parent+"."+self.name
        dec  = self.comment.buildMatlabComment(indent)
        dec += indent+"%-60s = %d;\n" % (fullname, self.value)
        dec += indent+"%-60s = %d;\n" % (parent+".rev{end+1,1}", self.value)
        dec += indent+"%-60s = '%s';" % (parent+".rev{end,  2}", self.name)
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        if len(parent) == 0:
            parent = "list"
        dec  = self.comment.buildPythonComment(indent)
        dec += "%s%s.addElement('%s', %d)"%(indent, parent, self.name, self.value)
        return dec



class listDefine(symbol):
    def __init__(self, node):
        symbol.__init__(self, node, "listDefine")
        self.entries = []
        self.lookup  = {}
        value = 0
        start = None
        for entry in node.getElementsByTagName("symbol"):
            ent = declareSymbol( entry )
            if ent.getType() == "listEntry":
                # does this have a value
                if ent.getValue() == None:
                    ent.setValue(value)
                else:
                    if self.lookup.has_key(ent.getValue()):
                        value = self.lookup[ent.getValue()].getValue()
                        ent.setValue(value)
                    else:
                        value = ent.getValue()
                if start == None:
                    start = value
                value += 1
            elif ent.getType() == "listNumEls":
                ent.setValue(value - start)
            else:
                raise Exception, "Incorrect entry '"+ent.getType()+"' found in enumeration:\n"+node.toxml()
            # save the entry
            self.entries.append(ent)
            self.lookup[ent.getName()] = ent


    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if len(parent) != 0:
            name = parent+"."+self.name
        name = re.sub("([a-z])([A-Z])", r"\1_\2",name).lower()
        dec  = indent+"// %s\n" % (name)
        dec += self.comment.buildKalComment(indent)
        for e in self.entries:
            dec += e.buildKalDeclaration(indent, name)+"\n"
        return dec

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if len(parent) != 0:
            name = parent.upper()+"_"+self.name
        dec  = ("/* %%-%ds */\n" % (defaultWidth-6)) % (name)
        dec += self.comment.buildCComment(indent+"  ")
        for e in self.entries:
            dec += e.buildCDeclaration("  ",name)+"\n"
        return dec

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if len(parent) != 0:
            name = parent+"."+self.name
        dec  = indent+"   %% %s\n" % (name)
        dec += self.comment.buildMatlabComment(indent+"   ")
        dec += indent+"   EC.%-60s = {};\n" % (name+".rev")
        off  = 0
        for e in self.entries:
            dec += e.buildMatlabDeclaration(indent+"   EC.", name, offset=off)+"\n"
            # does this reset the value, either way increment after
            if e.getValue() != None:
                off = e.getValue()
            off += 1
        return dec

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        name = self.name
        if len(parent) != 0:
            name = parent+"."+self.name
        dec  = indent+"# %s\n" % (name)
        dec += self.comment.buildPythonComment(indent)
        dec += indent+"%s = encorePdu.Lookup()\n" % (name)
        off  = 0
        for e in self.entries:
            dec += e.buildPythonDeclaration(indent, name, offset=off)+"\n"
            # does this reset the value, either way increment after
            if e.getValue() != None:
                off = e.getValue()
            off += 1
        return dec


class value:
    def __init__(self, value):
        self.value = value

    def getName(self):
        return ""

    def getValue(self):
        return self.value

    def getSize(self):
        return 1

    def getType(self):
        return "value"

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        # todo - do we even care about this?
        return ""

    def buildCDeclaration(self, indent="", parent="", **kwargs):
        return self.value

    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        # todo - do we even care about this?
        return ""

    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        # todo - do we even care about this?
        return ""


class ifdef(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "ifdef")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment("")
        return dec+self.__buildDeclaration("#", indent)

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"ifdef "+self.test

class ifndef(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "ifndef")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment("")
        return dec+self.__buildDeclaration("#", indent)

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"ifndef "+self.test

class hashIf(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "hashIf")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment("")
        return dec+self.__buildDeclaration("#", indent)

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"if "+self.test

class hashElse(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "hashElse")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment(indent)
        return dec+self.__buildDeclaration("#", "")

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"else"

class hashElif(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "hashElif")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment(indent)
        return dec+self.__buildDeclaration("#", "")

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"elif "+self.test


class endif(preproc):
    def __init__(self, node):
        preproc.__init__(self, node, "endif")

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildKalComment(indent)
        return dec+self.__buildDeclaration("#", indent)
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        dec = self.comment.buildCComment(indent)
        return dec+self.__buildDeclaration("#", "")

    def __buildDeclaration(self, pre, indent):
        return indent+pre+"endif"



class banner:
    def __init__(self, node):
        self.text = getTag(node, "text")
        self.text = self.text.replace("\n"," ")
        while self.text.find("  ") != -1:
            self.text = self.text.replace("  ", " ")
        self.text = self.text.split(" ")

    def getName(self):
        return ""

    def getType(self):
        return "banner"

    def buildKalDeclaration(self, indent="", parent="", **kwargs):
        width=defaultWidth
        if kwargs.has_key("width"):
            width = kwargs["width"]
        return self.__buildDeclaration(indent, width, "/*", "*/")
    def buildCDeclaration(self, indent="", parent="", **kwargs):
        width=defaultWidth
        if kwargs.has_key("width"):
            width = kwargs["width"]
        return self.__buildDeclaration(indent, width, "/*", "*/")
    def buildMatlabDeclaration(self, indent="", parent="", **kwargs):
        width=defaultWidth
        if kwargs.has_key("width"):
            width = kwargs["width"]
        return self.__buildDeclaration(indent, width, "%", "")
    def buildPythonDeclaration(self, indent="", parent="", **kwargs):
        width=defaultWidth
        if kwargs.has_key("width"):
            width = kwargs["width"]
        return self.__buildDeclaration(indent, width, "#", "")



    def __buildDeclaration(self, indent, width, comStart, comEnd):
        # first off put the big banner
        dec  = indent+comStart
        dec += "*" * (width - len(indent) - len(comStart) - len(comEnd))
        dec += comEnd+"\n"
        # build up the rest
        line = indent+comStart
        for t in self.text:
            # work out if this will push us over the limit, remember the space
            if (len(line)+len(t)+len(comEnd)+1) > width:
                line = ("%%-%ds%%s" % (width-len(comEnd))) % (line, comEnd)
                dec += line.rstrip()+"\n"
                line = indent+comStart+" "
            line += " "+t
        line = ("%%-%ds%%s" % (width-len(comEnd))) % (line, comEnd)
        dec += line.rstrip()+"\n"
        dec += indent+comStart
        dec += "*" * (width - len(indent) - len(comStart) - len(comEnd))
        dec += comEnd+"\n"
        return dec


TYPE_LUT["struct"]     = structure
TYPE_LUT["typedef"]    = typedef
TYPE_LUT["define"]     = define
TYPE_LUT["enum"]       = enum
TYPE_LUT["enumEntry"]  = enumEntry
TYPE_LUT["ifdef"]      = ifdef
TYPE_LUT["ifndef"]     = ifndef
TYPE_LUT["hashIf"]     = hashIf
TYPE_LUT["hashElse"]   = hashElse
TYPE_LUT["hashElif"]   = hashElif
TYPE_LUT["endif"]      = endif
TYPE_LUT["banner"]     = banner
TYPE_LUT["general"]    = general
TYPE_LUT["listDefine"] = listDefine
TYPE_LUT["listEntry"]  = listEntry
TYPE_LUT["listNumEls"] = listNumEls
TYPE_LUT["union"]      = union

def declareSymbol(node):
    type = getTag(node, "type")
    try:
        s = TYPE_LUT[getTag(node, "type")](node)
    except KeyError, e:
        raise Exception, "Unknown type "+repr(e)+" found in symbol:\n"+node.toxml()

    return s


def writeKalimbaOutput(output, data):
    # bumf required at the head of files that might be released
    output.write("/"+("*"*(defaultWidth-1))+"\n")
    output.write("\n")
    output.write("               (c) Cambridge Silicon Radio Limited 2009\n")
    output.write("\n")
    output.write("               All rights reserved and confidential information of CSR\n")
    output.write("\n")
    output.write("REVISION:      $Revision$\n")
    output.write(("*"*(defaultWidth-1))+"/\n")

    # bumf that says file was autogenerated
    output.write("/"+("*"*(defaultWidth-2))+"/\n")
    line = "/* This file was autogenerated from:"
    line = ("%%-%ds */" % (defaultWidth-3)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "/*     %s" % (inputFile)
    line = ("%%-%ds */" % (defaultWidth-3)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "/* %s" % ("Generated on "+datetime.date.today().strftime("%Y-%m-%d"))
    line = ("%%-%ds */" % (defaultWidth-3)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    # build guard bands
    output.write("/"+("*"*(defaultWidth-2))+"/\n\n")
    output.write("#ifndef "+filename+"\n")
    output.write("#define "+filename+"\n")
    for d in data:
        # now just print it out
        line = d.buildKalDeclaration("", "")
        if len(line) != 0:
            output.write(line+"\n\n")
    # close the guard band
    output.write("#endif\n")


def writeCOutput(output, data):
    # bumf required at the head of files that might be released
    output.write("/"+("*"*(defaultWidth-1))+"\n")
    output.write("\n")
    output.write("               (c) Cambridge Silicon Radio Limited 2009\n")
    output.write("\n")
    output.write("               All rights reserved and confidential information of CSR\n")
    output.write("\n")
    output.write("REVISION:      $Revision$\n")
    output.write(("*"*(defaultWidth-1))+"/\n")

    # bumf that says file was autogenerated
    output.write("/"+("*"*(defaultWidth-2))+"/\n")
    line = "/* This file was autogenerated (using genApi.py) from:"
    line = ("%%-%ds*/" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "/*     %s" % (inputFile)
    line = ("%%-%ds*/" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "/* %s" % ("Generated on "+datetime.date.today().strftime("%Y-%m-%d"))
    line = ("%%-%ds*/" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    # build guard bands
    output.write("/"+("*"*(defaultWidth-2))+"/\n\n")
    output.write("#ifndef "+filename+"\n")
    output.write("#define "+filename+"\n")
    for d in data:
        # now just print it out
        line = d.buildCDeclaration("", "")
        if len(line) != 0:
            output.write(line+"\n\n")
    # close the guard band
    output.write("#endif\n")


def writeMatlabOutput(output, data):
    # bumf that says file was autogenerated
    output.write("% "+("*"*(defaultWidth-2))+"\n")
    output.write("% This file was autogenerated from:\n")
    output.write("%%      %s\n" % (inputFile))
    output.write("%% %s\n" % ("Generated on "+datetime.date.today().strftime("%Y-%m-%d")))
    for d in data:
        # now just print it out
        line = d.buildMatlabDeclaration("", "")
        if len(line) != 0:
            output.write(line+"\n\n")

def writePythonOutput(output, data):
    # bumf that says file was autogenerated
    output.write("# "+("*"*(defaultWidth-2))+"\n")
    output.write("# This file was autogenerated from:\n")
    output.write("#      %s\n" % (inputFile))
    output.write("# %s\n" % ("Generated on "+datetime.date.today().strftime("%Y-%m-%d")))
    output.write("# "+("*"*(defaultWidth-2))+"\n\n\n")
    output.write("import encorePdu\n\n\n")
    for d in data:
        # now just print it out
        line = d.buildPythonDeclaration("", "")
        if len(line) != 0:
            output.write(line+"\n\n")


def writeInfoOutput(output, data):
    # bumf that says file was autogenerated
    output.write("<!"+("*"*(defaultWidth-3))+">\n")
    line = "<! This file was autogenerated from:"
    line = ("%%-%ds >" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "<!     %s" % (inputFile)
    line = ("%%-%ds >" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    line = "<! %s" % ("Generated on "+datetime.date.today().strftime("%Y-%m-%d"))
    line = ("%%-%ds >" % (defaultWidth-2)) % (line)
    line = line.rstrip()
    output.write(line+"\n")
    output.write("<!"+("*"*(defaultWidth-3))+">\n\n")
    # html gubbins
    output.write("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n")
    output.write("<html>\n")
    output.write("<head>\n")
    output.write("<title>EnCore2 documentation</title>\n")
    output.write("<h1>EnCore2 documentation</h1>\n")
    output.write("</head>\n")
    output.write("<body>\n")
    output.write("<p>This document describes the various API messages and structures\n")
    output.write("used by the EnCore 2 host component (MCHS) and the various onchip\n")
    output.write("components to communicate with each other.</p>\n")
    output.write("\n<table border=1>\n")
    output.write("  <tr>\n")
    output.write("    <th>Symbol</th>\n")
    output.write("    <th>Type</th>\n")
    output.write("    <th>Description</th>\n")
    output.write("  </tr>\n")

    for d in data:
        # now just print it out
        line = d.buildSimpleInfoDeclaration()
        if len(line) != 0:
            output.write(line+"\n\n")
    output.write("</table>\n<hr>\n")

    for d in data:
        # now just print it out
        line = d.buildFullInfoDeclaration(1)
        if len(line) != 0:
            output.write(line+"\n\n")


    output.write("</body>\n</html>\n")


def checkLookup(name):
    """Helper function to see if this symbol is actually one we
    have already defined"""
    if not SYMBOL_LUT.has_key(name):
        # we don't care
        return None
    # is it one we really care about
    t = SYMBOL_LUT[name].getType()
    if t == "typedef":
        t = SYMBOL_LUT[name].getAliasType()
    if t == "general" or t == "struct" or t == "union":
        return SYMBOL_LUT[name]



if __name__ == "__main__":
    if len(sys.argv) != 4:
        raise Exception("Wrong number of input arguments!")

    # rename the input arguments
    inputFile = sys.argv[1]
    format = sys.argv[2]
    filename = sys.argv[3]

    # parse the file
    dom = parse(inputFile)
    # clean up the format
    format = format.upper()

    # try and open the file
    try:
        output = open(filename, "w")
    except IOError:
        print "Unable to open the output file"
        exit(1)

    # sort the name out
    filename = filename.replace(".", "_").upper()
    filename = filename.replace("/", "__")
    filename = filename.replace(":", "__")
    filename = filename.replace("-", "__")

    data = []
    SYMBOL_LUT = {}

    for node in dom.childNodes[0].childNodes:
        # if it isn't an element node ignore
        if node.nodeType != node.ELEMENT_NODE:
            continue
        # is it a symbol
        if node.tagName != "symbol":
            continue

        # is this a structure
        try:
            s = declareSymbol(node)
        except TagError, e:
            print e()
            output.close()
            exit(1)
        except Exception, e:
            print e
            output.close()
            exit(1)

        data.append(s)
        SYMBOL_LUT[s.getName()] = s

    # no iterate over lookint to fix up the structure defines
    for s in data:
        # was this a typedef, if so do we need to change the kalimba define
        if s.getType() == "typedef":
            # did we know what the original is
            if s.define.getType() == "value":
                # we didn't recognise it, is it something we actually know about
                if SYMBOL_LUT.has_key(s.getDefineValue()):
                    s.setStruct(SYMBOL_LUT[s.getDefineValue()])



    if format == "KALIMBA":
        writeKalimbaOutput(output, data)
    elif format == "XAP":
        writeCOutput(output, data)
    elif format == "MATLAB":
        writeMatlabOutput(output, data)
    elif format == "INFO":
        writeInfoOutput(output, data)
    elif format == "PYTHON":
        writePythonOutput(output, data)
    else:
        raise Exception, "Unrecognised format type, must be one of 'kalimba', 'xap' or 'matlab'"


