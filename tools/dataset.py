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
            doc.output()

class Utterance:
    """Message of a conversation"""
    
    def __init__(self, id, message):
        """Initialize an Utterance"""
        self.m_id = id
        self.m_message = message

    def __str__(self):
        """String description of an utterance"""
        return "{}: {}".format(self.m_id, self.m_message)

class Conversation:
    """Conversation is a list of utterances"""

    def __init__(self):
        """Initialize a Conversation"""
        self.m_utterances= []
    
    def addUtterance(self, id, message):
        """Add a message to the conversation"""
        self.m_utterances.append(Utterance(id, message))

    def shuffle(self):
        """Shuffle messages in a conversation"""
        random.shuffle(self.m_utterances)

    def __str__(self):
        """String description of a conversation"""
        return '\n'.join(str(v) for v in self.m_utterances)


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

    def __str__(self):
        """String description of a dialog"""
        return '\n---\n'.join(str(v) for v in self.m_conversations)

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

    def output(self):
        """Print document to the standard output"""
        print('\n\n--- dialog separator ---\n\n'.join(str(v) for v in self.m_dialogs))

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
