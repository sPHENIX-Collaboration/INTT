class Person :
    def __init__( self, first="", family="", middle="", affiliations=[], priority=0 ) :
        self.first_name_ = first
        self.middle_name_ = middle
        self.family_name_ = family
        self.affiliations_ = affiliations
        self.affiliation_numbers_ = []
        self.priority_ = priority

    #operator for sort
    def __lt__( self, other ) :
        if not isinstance(other, Person ):
            return NotImplemented

        # for people who should appear first
        if self.priority_ != other.priority_ :
            return self.priority_ > other.priority_

        # comparison of family name first, then first name. I have no idea for middle name
        if self.family_name_ == other.family_name_ :
            return self.first_name_ < other.first_name_;

        return self.family_name_ < other.family_name_

    # Printing name in RIKEN APR format
    def PrintName( self, end="", is_last=False ) :
        print( "\\Name{" + self.first_name_ + "~", end="" )
        if self.middle_name_ != "" :
            print( self.middle_name_ +  "~", end="" )

        print( self.family_name_ + "}", end="" )
        # Trick not to write "," for only the last person
        if is_last is False :
            print( ",", end="" )

        print( "", end=end )
            

    # Printing institution list in RIKEN APR format
    def PrintInstitutions( self ) :
        for affiliation in self.affiliation_numbers_ :
            print( "\\institute{" + str(affiliation) + "}", end="" )

        print()

class People:
    def __init__( self ) :
        self.persons_ = []
        self.persons_.append( Person( first="X.",    family="Wei",        affiliations=["Purdue"          ] ) )
        self.persons_.append( Person( first="Y.",    family="Akiba",      affiliations=["RIKEN"           ] ) ) 	
        self.persons_.append( Person( first="J.",    family="Bertaux",    affiliations=["Purdue"          ] ) )
        self.persons_.append( Person( first="D.",    family="Cacace",     affiliations=["BNL"             ] ) )
        self.persons_.append( Person( first="R.",    family="Cecato",     affiliations=["BNL_Instr"       ], middle="G." ) )
        self.persons_.append( Person( first="A.",    family="Enokizono",  affiliations=["RIKEN"           ] ) ) 
        self.persons_.append( Person( first="K.",    family="Fujiki",     affiliations=["RIKEN", "Rikkyo" ] ) )
        self.persons_.append( Person( first="M.",    family="Fujiwara",   affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="T.",    family="Hachiya",    affiliations=["RIKEN", "NWU"    ] ) )
        self.persons_.append( Person( first="S.",    family="Hasegawa",   affiliations=["RIKEN", "JAEA"   ] ) )
        self.persons_.append( Person( first="M.",    family="Hata",       affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="B.",    family="Hong",       affiliations=["Korea"           ] ) )
        self.persons_.append( Person( first="J.",    family="Hwang",      affiliations=["Korea"           ] ) )
        self.persons_.append( Person( first="M.",    family="Ikemoto",    affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="R.",    family="Kan",        affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="M.",    family="Kano",       affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="T.",    family="Kato",       affiliations=["RIKEN", "Rikkyo" ] ) )
        self.persons_.append( Person( first="T.",    family="Kikuchi",    affiliations=["RIKEN", "Rikkyo" ] ) )
        self.persons_.append( Person( first="T.",    family="Kondo",      affiliations=["TIRI"            ] ) )
        self.persons_.append( Person( first="C.~M.", family="Kuo",        affiliations=["NCU"             ] ) )
        self.persons_.append( Person( first="R.~S.", family="Lu",         affiliations=["NTU"             ] ) )
        self.persons_.append( Person( first="N.",    family="Morimoto",   affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="I.",    family="Nakagawa",   affiliations=["RIKEN"           ] ) ) 
        self.persons_.append( Person( first="G.",    family="Nukazuka",   affiliations=["RIKEN"           ] ) )
        self.persons_.append( Person( first="R.",    family="Nouicer",    affiliations=["BNL"             ] ) )
        self.persons_.append( Person( first="R.",    family="Pisani",     affiliations=["BNL"             ] ) )
        self.persons_.append( Person( first="C.~W.", family="Shih",       affiliations=["NCU"             ] ) )
        self.persons_.append( Person( first="M.",    family="Shimomura",  affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="R.",    family="Shishikura", affiliations=["RIKEN", "Rikkyo" ] ) )
        self.persons_.append( Person( first="M.",    family="Stojanovic", affiliations=["Purdue"          ] ) )
        self.persons_.append( Person( first="Y.",    family="Sugiyama",   affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="W.~C.", family="Tang",       affiliations=["NCU"             ] ) )
        self.persons_.append( Person( first="Y.",    family="Terasaka",   affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="H.",    family="Tsujibata",  affiliations=["NWU"             ] ) )
        self.persons_.append( Person( first="M.",    family="Watanabe",   affiliations=["NWU"             ] ) )

        self.sorted_persons_ = []

        self.institution_dict = {
            "Purdue": "Department of Physics and Astronomy, Purdue University",
            "Rikkyo": "Department of Physics, Rikkyo University",
            "NWU": "Department of Mathematical and Physical Sciences, Nara Women's University",
            "NCU": "Department of Physics, National Central University",
            "NTU": "Department of Physics, National Taiwan University",
            "Korea": "Department of Physics, Korea University",
            "BNL_Instr": "Instrumentation Division, Brookhaven National Laboratory",
            "JAEA": "Advanced Science Research Center, Japan Atomic Energy Agency",
            "BNL": "Physics Department, Brookhaven National Laboratory",
            "RIKEN": "RIKEN Nishina Center",
            "TIRI": "Information Systems Technology Division, Tokyo Metropolitan Industrial Technology Research Institute"
        }
        self.sorted_affiliations_ = []
        
    def SetPriority( self, family_name, value ) :
        found = False
        for person in self.persons_ :
            if person.family_name_ == family_name :
                person.priority_ = value
                found = True
                break

        if found is False :
            message = "  Your family name is not in the default list. Are you new or due to typo?  "
            print( "\n" + "=" * len(message) ) 
            print( message )
            print( "=" * len(message), "\n" ) 
            return False

        return True

    def SetAffiliationNumbers( self ) :
        """
        Giving a number to each affiliation of people.
        """
        
        appearance_list = []
        number = 0

        # loop over people
        for person in self.sorted_persons_ :
            numbers = []

            # loop over affiliations of this person
            for affiliation in person.affiliations_ :
                
                # check whether this affiliation is in the list or not
                try :
                    index = appearance_list.index( affiliation )
                except ValueError:
                    index = -1

                # if this affiliation is not in the list, it's new. Give the largest number, and add it to the list
                if index  == -1 :
                    number += 1
                    numbers.append( number )
                    appearance_list.append( affiliation )
                else:
                    # if this affiliation is found, just give the index of it in the list
                    numbers.append( index+1 )

                person.affiliation_numbers_ = numbers
            # end of for affiliation in person.affiliations_ :
        # end of for person in self.sorted_persons_ :

    def MakeAffiliationList( self ) :
        self.sorted_affiliations= []

        # loop over sorted people
        for person in self.sorted_persons_ :
            # loop over affiliations of this person            
            for affiliation in person.affiliations_ :

                # if this affiliation is NOT in the sorted affiliation list, set -1 to the index variable
                try :
                    index = self.sorted_affiliations_.index( self.institution_dict[affiliation] )
                except ValueError:
                    index = -1

                # if this affiliation is NOT in the list, it's new. Add it to the sorted alliation list.
                if index  == -1 :
                    self.sorted_affiliations_.append( self.institution_dict[affiliation] )

        
    def PrintForAPR( self ) :
        self.sorted_persons_ = sorted( self.persons_ )
        self.SetAffiliationNumbers()

        ############################################################
        # List of people
        ############################################################
        # For example:
        # \author{
	#    \Name{Y.~Akiba},\institute{1}
        #    and
	#    \Name{J.~Bertaux},\institute{2}
        # }
        ############################################################
        print( "\\author{" )
        for i, person in enumerate( self.sorted_persons_ ) :
            if i == len( self.sorted_persons_ ) - 1 :
                print( "\tand" )

            print( "\t", end="" )
            person.PrintName( end="", is_last=(i+1==len(self.sorted_persons_) ) )
            person.PrintInstitutions()

        print( "}\n" ) # closure
        
        ############################################################
        # List of institutions
        ############################################################
        # For example: 
        # \INSTITUTE{1}{RIKEN Nishina Center}
        # \INSTITUTE{2}{Department of Physics and Astronomy, Purdue University}
        ############################################################
        self.MakeAffiliationList()
        
        for i, institution in enumerate( self.sorted_affiliations_ ) :
            print( "\\INSTITUTE{" + str(i+1) + "}{" + institution + "}" )
    
if __name__ == "__main__" :
    print( "Who are you?" )
    family_name = input( "Type your family name (eg. Nukazuka) > " )

    # it's for debugging
    if family_name == "" : 
        family_name = "Nukazuka"

    
    people = People()
    found = people.SetPriority( family_name, 1 )
    
    # If given family name is found, print the author list
    # Here, I assumed that each has unique family name. If not, change the order of self.persons_.append( .... ). in this code.
    if found is True :
        people.PrintForAPR()
