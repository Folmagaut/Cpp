
// Generated from Formula.g4 by ANTLR 4.13.2


#include "FormulaLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct FormulaLexerStaticData final {
  FormulaLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FormulaLexerStaticData(const FormulaLexerStaticData&) = delete;
  FormulaLexerStaticData(FormulaLexerStaticData&&) = delete;
  FormulaLexerStaticData& operator=(const FormulaLexerStaticData&) = delete;
  FormulaLexerStaticData& operator=(FormulaLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag formulalexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<FormulaLexerStaticData> formulalexerLexerStaticData = nullptr;

void formulalexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (formulalexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(formulalexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<FormulaLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "INT", "UINT", "EXPONENT", "NUMBER", "ADD", "SUB", 
      "MUL", "DIV", "CELL", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'('", "')'", "", "'+'", "'-'", "'*'", "'/'"
    },
    std::vector<std::string>{
      "", "", "", "NUMBER", "ADD", "SUB", "MUL", "DIV", "CELL", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,9,81,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,
  	2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,1,0,1,0,1,1,1,1,1,2,3,2,31,
  	8,2,1,2,1,2,1,3,4,3,36,8,3,11,3,12,3,37,1,4,1,4,1,4,1,5,1,5,3,5,45,8,
  	5,1,5,3,5,48,8,5,1,5,1,5,1,5,3,5,53,8,5,3,5,55,8,5,1,6,1,6,1,7,1,7,1,
  	8,1,8,1,9,1,9,1,10,4,10,66,8,10,11,10,12,10,67,1,10,4,10,71,8,10,11,10,
  	12,10,72,1,11,4,11,76,8,11,11,11,12,11,77,1,11,1,11,0,0,12,1,1,3,2,5,
  	0,7,0,9,0,11,3,13,4,15,5,17,6,19,7,21,8,23,9,1,0,5,2,0,43,43,45,45,1,
  	0,48,57,2,0,69,69,101,101,1,0,65,90,3,0,9,10,13,13,32,32,86,0,1,1,0,0,
  	0,0,3,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,
  	1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,1,25,1,0,0,0,3,27,1,0,0,0,5,30,1,0,
  	0,0,7,35,1,0,0,0,9,39,1,0,0,0,11,54,1,0,0,0,13,56,1,0,0,0,15,58,1,0,0,
  	0,17,60,1,0,0,0,19,62,1,0,0,0,21,65,1,0,0,0,23,75,1,0,0,0,25,26,5,40,
  	0,0,26,2,1,0,0,0,27,28,5,41,0,0,28,4,1,0,0,0,29,31,7,0,0,0,30,29,1,0,
  	0,0,30,31,1,0,0,0,31,32,1,0,0,0,32,33,3,7,3,0,33,6,1,0,0,0,34,36,7,1,
  	0,0,35,34,1,0,0,0,36,37,1,0,0,0,37,35,1,0,0,0,37,38,1,0,0,0,38,8,1,0,
  	0,0,39,40,7,2,0,0,40,41,3,5,2,0,41,10,1,0,0,0,42,44,3,7,3,0,43,45,3,9,
  	4,0,44,43,1,0,0,0,44,45,1,0,0,0,45,55,1,0,0,0,46,48,3,7,3,0,47,46,1,0,
  	0,0,47,48,1,0,0,0,48,49,1,0,0,0,49,50,5,46,0,0,50,52,3,7,3,0,51,53,3,
  	9,4,0,52,51,1,0,0,0,52,53,1,0,0,0,53,55,1,0,0,0,54,42,1,0,0,0,54,47,1,
  	0,0,0,55,12,1,0,0,0,56,57,5,43,0,0,57,14,1,0,0,0,58,59,5,45,0,0,59,16,
  	1,0,0,0,60,61,5,42,0,0,61,18,1,0,0,0,62,63,5,47,0,0,63,20,1,0,0,0,64,
  	66,7,3,0,0,65,64,1,0,0,0,66,67,1,0,0,0,67,65,1,0,0,0,67,68,1,0,0,0,68,
  	70,1,0,0,0,69,71,7,1,0,0,70,69,1,0,0,0,71,72,1,0,0,0,72,70,1,0,0,0,72,
  	73,1,0,0,0,73,22,1,0,0,0,74,76,7,4,0,0,75,74,1,0,0,0,76,77,1,0,0,0,77,
  	75,1,0,0,0,77,78,1,0,0,0,78,79,1,0,0,0,79,80,6,11,0,0,80,24,1,0,0,0,10,
  	0,30,37,44,47,52,54,67,72,77,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  formulalexerLexerStaticData = std::move(staticData);
}

}

FormulaLexer::FormulaLexer(CharStream *input) : Lexer(input) {
  FormulaLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *formulalexerLexerStaticData->atn, formulalexerLexerStaticData->decisionToDFA, formulalexerLexerStaticData->sharedContextCache);
}

FormulaLexer::~FormulaLexer() {
  delete _interpreter;
}

std::string FormulaLexer::getGrammarFileName() const {
  return "Formula.g4";
}

const std::vector<std::string>& FormulaLexer::getRuleNames() const {
  return formulalexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& FormulaLexer::getChannelNames() const {
  return formulalexerLexerStaticData->channelNames;
}

const std::vector<std::string>& FormulaLexer::getModeNames() const {
  return formulalexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& FormulaLexer::getVocabulary() const {
  return formulalexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView FormulaLexer::getSerializedATN() const {
  return formulalexerLexerStaticData->serializedATN;
}

const atn::ATN& FormulaLexer::getATN() const {
  return *formulalexerLexerStaticData->atn;
}




void FormulaLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  formulalexerLexerInitialize();
#else
  ::antlr4::internal::call_once(formulalexerLexerOnceFlag, formulalexerLexerInitialize);
#endif
}
