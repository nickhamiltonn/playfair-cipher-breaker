import math

def decrypt(c_text, key):
    p_text = ""
    
    for i in range(math.floor(len(c_text) / 2)):
        pair_x = c_text[2*i: 2*i +2]
        p_text += key[pair_x]

    return p_text

def swap_key_pairs(key, i_1, i_2):
    key_list = list(key)
    key[key_list[i_1]], key[key_list[i_2]] = key[key_list[i_2]], key[key_list[i_1]]

def scoring_fn(plain_text):
    bigram_percentages = {'TH': 116997844, 'HE': 100689263, 'IN': 87674002, 'ER': 77134382, 'AN': 69775179, 'RE': 60923600, 'ES': 57070453, 'ON': 56915252, 'ST': 54018399, 'NT': 50701084, 'EN': 48991276, 'AT': 48274564, 'ED': 46647960, 'ND': 46194306, 'TO': 46115188, 'OR': 45725191, 'EA': 43329810, 'TI': 42888666, 'AR': 42353262, 'TE': 42295813, 'NG': 38567365, 'AL': 38211584, 'IT': 37938534, 'AS': 37773878, 'IS': 37349981, 'HA': 35971841, 'ET': 32872552, 'SE': 31532272, 'OU': 31112284, 'OF': 30540904, 'LE': 30383262, 'SA': 30080131, 'VE': 29320973, 'RO': 29230770, 'RA': 28645577, 'RI': 27634643, 'HI': 27495342, 'NE': 27331675, 'ME': 27237733, 'DE': 27029835, 'CO': 26737101, 'TA': 26147593, 'EC': 25775798, 'SI': 25758841, 'LL': 24636875, 'SO': 23903631, 'NA': 23547524, 'LI': 23291169, 'LA': 23178317, 'EL': 23092248, 'MA': 21828378, 'DI': 21673998, 'IC': 21468412, 'RT': 21456059, 'NS': 21306421, 'RS': 21237259, 'IO': 21210160, 'OM': 21066156, 'CH': 20132750, 'OT': 20088048, 'CA': 19930754, 'CE': 19803619, 'HO': 19729026, 'BE': 19468489, 'TT': 19367472, 'FO': 18923772, 'TS': 18922522, 'SS': 18915696, 'NO': 18894111, 'EE': 18497942, 'EM': 18145294, 'AC': 17904683, 'IL': 17877600, 'DA': 17584055, 'NI': 17452104, 'UR': 17341717, 'WA': 16838794, 'SH': 16773127, 'EI': 16026915, 'AM': 15975981, 'TR': 15821226, 'DT': 15759673, 'US': 15699353, 'LO': 15596310, 'PE': 15573318, 'UN': 15237699, 'NC': 15214623, 'WI': 15213018, 'UT': 15137169, 'AD': 14877234, 'EW': 14776406, 'OW': 14610429, 'GE': 14425023, 'EP': 14024377, 'AI': 13974919, 'LY': 13742031, 'OL': 13726491, 'FT': 13696078, 'OS': 13596265, 'EO': 13524186}

    bigrams = {}

    for bigram in bigram_percentages.keys():
        bigrams[bigram.upper()] = (bigram_percentages[bigram] / 4333253481) * len(plain_text)

    # Break text into bigrams
    plain_text_bigrams = []
    i = 0
    while (i < len(plain_text)):
        plain_text_bigrams.append(plain_text[i:i+2])
        i += 2

    # Give a count for each bigram in the plaintext
    plain_text_counts = {}
    for bigram in plain_text_bigrams:
        plain_text_counts[bigram] = plain_text_bigrams.count(bigram)

    chi_squared_sum = 0
    for bigram in plain_text_counts:
        if (bigram in plain_text_counts.keys()):
            chi_squared_sum += math.sqrt((plain_text_counts[bigram] - bigrams[bigram]) ** 2 / bigrams[bigram])
        else:
            chi_squared_sum += math.sqrt((0 - bigrams[bigram]) ** 2 / bigrams[bigram])

    score = chi_squared_sum / len(bigrams)
    return score


def bad_scoring_fn(plain_text, freq_map):
    starting_score = 10000 + 675
    multiplier = 1

    for gram in freq_map.keys():
        multiplier *= 2
        for word in freq_map[gram].keys():
            if word.lower() in plain_text:
                starting_score -= 1 * multiplier

    return starting_score

    

def create_freq_map():
    
    def create_gram_dict(filename, d):
        with open(filename) as f:
            lines = f.readlines()
            
            for line in lines:
                words = line.split()
                d.update({words[0]: int(words[1])})
    
    expected_freq = dict()

    bigram = dict()
    trigram = dict()
    quadgram = dict()
    quintgram = dict()
    words = dict()

    create_gram_dict("frequencies/bigrams.txt", bigram)
    create_gram_dict("frequencies/trigrams.txt", trigram)
    create_gram_dict("frequencies/quadgrams.txt", quadgram)
    create_gram_dict("frequencies/quintgrams.txt", quintgram)
    create_gram_dict("frequencies/words.txt", words)
    
    expected_freq.update({"bi": bigram})
    expected_freq.update({"tri": trigram})
    expected_freq.update({"quad": quadgram})
    expected_freq.update({"quint": quintgram})
    expected_freq.update({"words": words})

    return expected_freq