import logging
import pprint
import sys

class Printer() :
    def __init__( self ) :
        self.lines = []
        self.separator = ""
        self.separator_sign = "---separator---"
        self.wall = '|'
        self.junction = '+'
        self.line = '-'
        self.left_margin_length = 1
        self.right_margin_length = 3
        self.max_length = 0

        Printer.color_dict = {
            "Black" : "0;30m",
            "Red"   : "0;31m",
            "Green" : "0;32m",
            "Yellow": "0;33m",
            "Blue"  : "0;34m",
            "Purple": "0;35m",
            "Cyan"  : "0;36m",
            "White" : "0;37m"
        }
        Printer.color_starter = '\033['
        Printer.color_closure = Printer.color_starter + Printer.color_dict[ "White" ]

    @staticmethod
    def ColorIt( words, color='Red' ) :
        if color is None :
            words = words
        elif color not in Printer.color_dict :
            print( "class Printer.ColorIt, Given color", color, "is not found in the list.", file=sys.stderr , flush=True )
        else:
            words = Printer.color_starter + Printer.color_dict[ color ] + words + Printer.color_closure
            
        return words

    @staticmethod
    def ColorBoolean( words ) :
        if words is True :
            return Printer.ColorIt( str(words), 'Green' )
        elif words is False :
            return Printer.ColorIt( str(words), 'Red' )


        # If words is not both of True and False, just return it
        return words

    def CountColorSign( self, words ) :
        num = 0
        # loop over all coloring words to count the number of their occurrences
        for key in self.color_dict :
            count_num = words.count( self.color_dict[key] )
            # somehow this modification works well....
            num += count_num * ( len( self.color_dict[key] ) + len(self.color_starter) )
            
        return num
    
    def AddLine( self, *all_words, sep=' ', color=None ) :
        line = ""
        for words in all_words :
            words = Printer.ColorBoolean( words )
                
            line += str(words) + sep
        self.lines.append( Printer.ColorIt(line, color) )

    def AddSeparator( self ) :
        self.lines.append( self.separator_sign )

    def Clear( self ) :
        self.lines = []
        
    def PrintOuterLine( self ) : 
        print( self.junction, 
               self.line * (self.max_length + self.left_margin_length + self.right_margin_length ),
               self.junction,
               sep='', flush=True )
        
    def Print( self, color="" ) :
        if len(self.lines) == 0 :
            return None
        
        if color != "" :
            print( Printer.color_starter + Printer.color_dict[ color ], end="" )
            
        self.max_length = len( max( self.lines, key=len ) )
        self.separator = self.line * self.max_length
        
        self.PrintOuterLine()
        for line in self.lines :
            if color != "" :
                print( Printer.color_starter + Printer.color_dict[ color ], end="" )

            # replace the separator sign with the separator
            if self.separator_sign in line :
                line = self.separator

            # If these words are colored,
            length_correction = self.CountColorSign( line )
            #if self.color_closure in line :
            #    length_correction = len( self.color_starter + self.color_dict[ "Black" ] + self.color_closure )
                
            print( self.wall, 
                   " " * self.left_margin_length,
                   line,
                   " " * (self.max_length - len(line) + length_correction ),
                   " " * self.right_margin_length,
                   self.wall,
                   sep='', flush=True )

        if color != "" :
            print( Printer.color_starter + Printer.color_dict[ color ], end="" )

        self.PrintOuterLine()
        if color != "" :
            print( Printer.color_closure, end="" )

