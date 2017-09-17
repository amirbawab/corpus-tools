import random
import argparse
from xml.dom import minidom

class CLI:
    def __init__(self):
        """Initialize a command line interface"""

        # Define arguments
        parser = argparse.ArgumentParser(description='ML Dialog tool parses and manipulates dialog data')
        parser.add_argument('-l','--loadXML', nargs=1, help='Dialog XML file')
        parser.add_argument('-s','--shuffle', action='store_true', help='Shuffle messages')
        parser.add_argument('-o','--output', action='store_true', help='Output messages')
        args = parser.parse_args()
        
        # Load XML
        dialog = Dialog()
        if args.loadXML is not None:
            dialog.loadXML(args.loadXML[0])

        # Shuffle messages
        if args.shuffle is True:
            dialog.shuffle()

        # Output messages
        if args.output is True:
            dialog.output()

class Dialog:
    """A dialog structure"""
    m_messages = []

    def loadXML(self, fileName):
        """Load Dialog XML file"""
        document = minidom.parse(fileName)
        dialogs = document.getElementsByTagName("dialog")
        for dialog in dialogs:
            conversations = dialog.getElementsByTagName("s")
            for conversation in conversations:
                utterances = conversation.getElementsByTagName("utt")
                for utterance in utterances:
                    self.m_messages.append(self.__getNodeText(utterance))

    def output(self):
        """Print dialog to the standard output"""
        for message in self.m_messages:
            print(message)

    def shuffle(self):
        """Shuffle messages"""
        random.shuffle(self.m_messages)


    def __getNodeText(self, node):
        """Extract text child node from a tag"""
        nodelist = node.childNodes
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                return node.data
        return ""

# Create a CLI instance
cli = CLI()
