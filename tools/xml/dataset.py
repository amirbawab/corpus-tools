import random
import argparse
from xml.dom import minidom

class CLI:
    def read(self):
        """Initialize a command line interface"""

        # Define arguments
        parser = argparse.ArgumentParser(description='Dataset tool. Parse and manipulate dialog data')
        parser.add_argument('-l','--loadXML', nargs='+', help='Dialog XML file')
        parser.add_argument('-s','--shuffle', action='store_true', help='Shuffle messages')
        parser.add_argument('-o','--output', action='store_true', help='Output messages')
        parser.add_argument('-O','--outputXML', action='store_true', help='Output messages in XML format')
        args = parser.parse_args()
        
        # Load XML
        doc = Document()
        if args.loadXML is not None:
            for xmlFile in args.loadXML:
                doc.loadXML(xmlFile)

        # Shuffle messages
        if args.shuffle is True:
            doc.shuffle()

        # Output messages
        if args.output is True:
            print(doc.toText())

        elif args.outputXML is True:
            print(doc.toXML())

class Utterance:
    """Message of a conversation"""
    
    def __init__(self, id, message):
        """Initialize an Utterance"""
        self.m_id = id
        self.m_message = message

    def toText(self):
        """String description of an utterance"""
        return "{}: {}".format(self.m_id, self.m_message)

    def toXML(self):
        """Convert utterance to <utt>"""
        return '        <utt uid="' + str(self.m_id) + '">' + str(self.m_message) + '</utt>\n'

class Conversation:
    """Conversation is a list of utterances"""

    def __init__(self):
        """Initialize a Conversation"""
        self.m_utterances= []
        self.dict = {}
        self.speakers = 1
    
    def addUtterance(self, id, message):
        """Add a message to the conversation"""
        if not id in self.dict.keys():
            self.dict[id] = self.speakers
            self.speakers += 1

        id = self.dict[id]

        self.m_utterances.append(Utterance(id, message))

    def shuffle(self):
        """Shuffle messages in a conversation"""
        random.shuffle(self.m_utterances)

    def toText(self):
        """String description of a conversation"""
        return '\n'.join(v.toText() for v in self.m_utterances)

    def toXML(self):
        """Convert conversation to XML"""
        code = "    <s>\n"
        for utterance in self.m_utterances:
            code += utterance.toXML()
        code += "    </s>\n"
        return code

class Dialog:
    """A dialog structure"""

    def __init__(self):
        """Initialize a Dialog"""
        self.m_conversations = []
    
    def addConversation(self, conversation):
        """Add a conversation to the dialog"""
        self.m_conversations.append(conversation)

    def shuffle(self):
        """Shuffle conversations, and recursively shuffle nested components"""
        for conversation in self.m_conversations:
            conversation.shuffle()
        random.shuffle(self.m_conversations)

    def toXML(self):
        """Convert dialog to xml"""
        code = "<dialog>\n"
        for conversation in self.m_conversations:
            code += conversation.toXML()
        code += "</dialog>\n"
        return code

    def toText(self):
        """String description of a dialog"""
        return '\n---\n'.join(v.toText() for v in self.m_conversations)

class Document:
    """Document for dialogs"""

    def __init__(self):
        """Inititlaize a Document"""
        self.m_dialogs = []

    def loadXML(self, fileName):
        """Load Dialog XML file"""

        # Parse XML document
        documentXML = minidom.parse(fileName)

        # Loop on all <dialog>
        dialogsXML = documentXML.getElementsByTagName("dialog")
        for dialogXML in dialogsXML:
            dialog = Dialog()

            # Loop on all <s>
            conversationsXML = dialogXML.getElementsByTagName("s")
            for conversationXML in conversationsXML:
                conversation = Conversation()
                
                # Loop on all <utt>
                utterancesXML = conversationXML.getElementsByTagName("utt")
                for utteranceXML in utterancesXML:
                    id = utteranceXML.getAttribute("uid")
                    text = self.__getNodeText(utteranceXML)
                    conversation.addUtterance(id, text)
                dialog.addConversation(conversation)
            self.m_dialogs.append(dialog)

    def shuffle(self):
        """Shuffle dialogs, and recursively shuffle nested components"""
        for dialog in self.m_dialogs:
            dialog.shuffle()
        random.shuffle(self.m_dialogs)

    def toText(self):
        """Convert document to text"""
        return '\n\n--- dialog separator ---\n\n'.join(v.toText() for v in self.m_dialogs)

    def toXML(self):
        """Convert document to XML"""
        code = '<?xml version="1.0"?>\n'
        for dialog in self.m_dialogs:
            code += dialog.toXML()
        return code

    def __getNodeText(self, node):
        """Extract text child node from a tag"""
        nodelist = node.childNodes
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                return node.data
        return ""

# Read user input
cli = CLI()
cli.read()
