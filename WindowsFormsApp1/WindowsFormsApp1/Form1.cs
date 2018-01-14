using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public List<Player> players = new List<Player>();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private ControlType GetPlayerControl<ControlType>(string controlPostFix, int playerIndex) where ControlType : Control
        {
            Control[] foundControls = this.Controls.Find("player" + playerIndex + controlPostFix, true);
            if (foundControls.Count() == 0)
                return null;
            else
                return (ControlType)foundControls[0];
        }
        
        public void UpdatePlayersList()
        {
            players.Clear();
            // Run through each of the player types
            for (int i = 1; i <= 8; ++i)
            {
                CheckBox playerCheckBox = GetPlayerControl<CheckBox>("CheckBox", i);
                // Only proceed if this player is checked
                if (playerCheckBox != null && playerCheckBox.Checked)
                {
                    Player newPlayer = new Player();
                    newPlayer.name = GetPlayerControl<TextBox>("NameBox", i).Text;
                    newPlayer.mmr = Int32.Parse(GetPlayerControl<TextBox>("MMRBox", i).Text);
                    newPlayer.score = Int32.Parse(GetPlayerControl<TextBox>("ScoreBox", i).Text);
                    newPlayer.team = (GetPlayerControl<ComboBox>("TeamBox", i).Text == "ALPHA") ? TEAM.ALPHA : TEAM.BRAVO;

                    players.Add(newPlayer);
                }
            }
        }

        public void OutputText(string newText)
        {
            resultsBox.AppendText(newText + Environment.NewLine);
        }

        private void checkBtn_Click(object sender, EventArgs e)
        {
            UpdatePlayersList();

            resultsBox.AppendText(Environment.NewLine + "---------------" + DateTime.Now.ToLongTimeString() + "---------------" + Environment.NewLine);

            // Check to see whether it is using Score, Matching rate, or both
            bool usingScore = false;
            bool usingMMR = false;
            // ...
            usingScore = true;
            usingMMR = true;
            // ...

            if (usingScore != usingMMR)
            {
                // Grab the result
                List<List<ValueClass>> result = GetAllCombinations_Single(players,usingScore);

                // Print em all out
                for (int i = 0; i < result.Count(); ++i)
                {
                    OutputText("Possibility " + i.ToString());
                    foreach(ValueClass val in result[i])
                    {
                        OutputText(val.Print());
                    }
                }

                // Then check against your set
                bool isCorrect = false;
                for (int i = 0; i < result.Count(); ++i)
                {
                    List<ValueClass> currResult = result[i];
                    // Our objective is to keep finding the relevant player in the result, and stuff it into the new list
                    List<Player> oldPlayerList = players.ConvertAll(player => player.Clone());
                    List<Player> finalPlayerList = new List<Player>();
                    // Iterate through all the Values
                    for (int j = 0; j < currResult.Count(); ++j)
                    {
                        // Find it in the player
                        bool foundPlayer = false;
                        for (int k = 0; k < oldPlayerList.Count(); ++k)
                        {
                            if (((usingScore && oldPlayerList[k].score == currResult[j].val) || (usingMMR && oldPlayerList[k].mmr == currResult[j].val)) &&
                                currResult[j].team == oldPlayerList[k].team)
                            {
                                finalPlayerList.Add(oldPlayerList[k].Clone());
                                oldPlayerList.RemoveAt(k);
                                foundPlayer = true;
                                break;
                            }
                        }

                        // Failed to find the player in the values, this is not the correct one
                        if (!foundPlayer)
                            break;
                    }

                    // YUP
                    if (finalPlayerList.Count() == players.Count())
                    {
                        OutputText("CORRECT! IT IS POSSIBILITY " + i.ToString());
                        isCorrect = true;
                        break;
                    }
                }

                if (!isCorrect)
                {
                    OutputText("WRONG! NO POSSIBILTY FOUND!");
                }

            }
            else
            {
                // First, arrange them all according to score
                List<Player> newPlayerList = players.ConvertAll(player => player.Clone());
                newPlayerList.Sort(delegate (Player a, Player b)
                {
                    return b.score.CompareTo(a.score);
                });

                // Then get the amount of players who have score
                int playersWithScore = 0;
                foreach(Player player in newPlayerList)
                {
                    if (player.score >= 0)
                        playersWithScore += 1;
                    else
                        break;
                }

                // Do the single sorting with the first batch (which is basically playersWithScore/2, but rounded down)
                List<Player> playersWithScoreList = newPlayerList.GetRange(0, playersWithScore / 2);
                List<Player> playersWithoutScoreList = newPlayerList.GetRange(playersWithScore/2, newPlayerList.Count() - playersWithScore/2);
                // Sort the withoutscore guys by matching rate
                playersWithoutScoreList.Sort(delegate (Player a, Player b)
                {
                    return b.mmr.CompareTo(a.mmr);
                });

                // Grab the results
                List<List<ValueClass>> firstResults = GetAllCombinations_Single(playersWithScoreList, true);
                // Convert it into a list of list of doubleValueClasses
                List<List<DoubleValueClass>> finalResults = new List<List<DoubleValueClass>>();
                foreach(List<ValueClass> valueClassList in firstResults)
                {
                    List<DoubleValueClass> convertedList = valueClassList.ConvertAll(val => val.ConvertToDoubleValue());
                    // Stuff in the extras
                    foreach(Player player in playersWithoutScoreList)
                    {
                        DoubleValueClass dbl = player.ConvertToDoubleValue();
                        dbl.team = TEAM.NONE;
                        convertedList.Add(dbl);
                    }
                    finalResults.Add(convertedList);
                }

                // Kay, so now all the current results have a full list, now do step 4 for each of the found steps
                int firstCount = finalResults.Count();
                for (int i = 0; i < firstCount; ++i)
                {
                    _Double_Step4(ref finalResults, i);
                }

                // Now it should be done

                // Print em all out
                for (int i = 0; i < finalResults.Count(); ++i)
                {
                    OutputText("Possibility " + i.ToString());
                    foreach (DoubleValueClass val in finalResults[i])
                    {
                        OutputText(val.Print());
                    }
                }

                // Then check against your set
                bool isCorrect = false;
                for (int i = 0; i < finalResults.Count(); ++i)
                {
                    List<DoubleValueClass> currResult = finalResults[i];
                    // Our objective is to keep finding the relevant player in the result, and stuff it into the new list
                    List<Player> oldPlayerList = players.ConvertAll(player => player.Clone());
                    List<Player> finalPlayerList = new List<Player>();
                    // Iterate through all the Values
                    for (int j = 0; j < currResult.Count(); ++j)
                    {
                        // Find it in the player
                        bool foundPlayer = false;
                        for (int k = 0; k < oldPlayerList.Count(); ++k)
                        {
                            if (oldPlayerList[k].score == currResult[j].score &&
                                oldPlayerList[k].mmr == currResult[j].mmr &&
                                oldPlayerList[k].team == currResult[j].team)
                            {
                                finalPlayerList.Add(oldPlayerList[k].Clone());
                                oldPlayerList.RemoveAt(k);
                                foundPlayer = true;
                                break;
                            }
                        }

                        // Failed to find the player in the values, this is not the correct one
                        if (!foundPlayer)
                            break;
                    }

                    // YUP
                    if (finalPlayerList.Count() == players.Count())
                    {
                        OutputText("CORRECT! IT IS POSSIBILITY " + i.ToString());
                        isCorrect = true;
                        break;
                    }
                }

                if (!isCorrect)
                {
                    OutputText("WRONG! NO POSSIBILTY FOUND!");
                }

            }
        }

        // 	7) Moving down the sorted list from the top, the players are placed into the team that has the lower total Matching Rate.							
        // Note: If one team becomes full, all of the remaining players are placed in the other team.
        // Note: Since the players are assigned teams in order, the total for the Matching Rate for both teams can become equal.
        // When that happens, the next player to be assigned will be placed into a randomly selected team.

        private void _Double_Step4(ref List<List<DoubleValueClass>> resultsList, int stepIndex)
        {
            List<DoubleValueClass> playerList = resultsList[stepIndex];

            // Recursively set the players, so that it can handle the branching case
            _Double_Step4_Recursive(ref resultsList, stepIndex, 2);

        }

        private void _Double_Step4_Recursive(ref List<List<DoubleValueClass>> resultsList, int stepIndex, int currIndex)
        {
            List<DoubleValueClass> playerList = resultsList[stepIndex];
            // If the currIndex is out of index, DONE
            if (currIndex >= playerList.Count())
                return;

            // If not, then just set it up
            // Grab the total score of both teams first
            int currALPHAScore = 0;
            int currBRAVOScore = 0;
            int totalALPHAPlayers = 0;
            int totalBRAVOPlayers = 0;
            for (int i = 0; i < playerList.Count(); ++i)
            {
                if (playerList[i].team == TEAM.ALPHA)
                {
                    currALPHAScore += playerList[i].mmr;
                    totalALPHAPlayers += 1;
                }
                else if (playerList[i].team == TEAM.BRAVO)
                {
                    currBRAVOScore += playerList[i].mmr;
                    totalBRAVOPlayers += 1;
                }

            }

            // If one of the teams is filled up, JUST STUFF EM IN AND DONE
            if (totalALPHAPlayers == playerList.Count() / 2)
            {
                foreach (DoubleValueClass val in playerList)
                {
                    if (val.team == TEAM.NONE)
                        val.team = TEAM.BRAVO;
                }
                return;
            }
            else if (totalBRAVOPlayers == playerList.Count() / 2)
            {
                foreach (DoubleValueClass val in playerList)
                {
                    if (val.team == TEAM.NONE)
                        val.team = TEAM.ALPHA;
                }
                return;
            }

            // Then just grab the current index and stuff him in
            if (currALPHAScore > currBRAVOScore)
            {
                playerList[currIndex].team = TEAM.BRAVO;
                _Double_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);
            }
            else if (currALPHAScore < currBRAVOScore)
            {
                playerList[currIndex].team = TEAM.ALPHA;
                _Double_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);

            }
            // OH BOTH ARE THE SAME, RUN BOTH
            else
            {
                // Duplicate the current list first
                List<DoubleValueClass> newList = playerList.ConvertAll(value => value.Clone());
                // Set the old list along its way
                playerList[currIndex].team = TEAM.ALPHA;
                _Double_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);
                // Then recurse through the new list
                resultsList.Add(newList);
                _Double_Step4_Recursive(ref resultsList, resultsList.Count - 1, currIndex + 1);
            }
        }

        // This function will return you all possible combinations of the teams
        public List<List<ValueClass>> GetAllCombinations_Single(List<Player> playerList, bool usingScore)
        {
            // Create the base case from the current sample first
            List<ValueClass> baseCase = new List<ValueClass>();

            if (usingScore)
            {
                OutputText("Using Score!");
                baseCase = playerList.ConvertAll(player => player.ConvertToValue(true));
            }
            else
            {
                OutputText("Using MMR!");
                baseCase = playerList.ConvertAll(player => player.ConvertToValue(false));
            }

            // Set all their teams to none first
            foreach (ValueClass val in baseCase)
            {
                val.team = TEAM.NONE;
            }

            // Sort according to their score
            baseCase.Sort(delegate (ValueClass a, ValueClass b)
            {
                return b.val.CompareTo(a.val);
            });
            // Begin the check
            return _Single_Step1(baseCase);
            
        }

        // 	1) The player with the highest Matching Rate is selected and placed in one of the teams (chosen randomly between ALPHA and BRAVO, referred to as Team 1).
        // Note: If there are multiple players with an identical highest Matching Rate then one is selected at random and placed in Team 1.
        private List<List<ValueClass>> _Single_Step1(List<ValueClass> startingInput)
        {
            List<List<ValueClass>> result = new List<List<ValueClass>>();

            // Just grab the first one and place him in one of the teams
            List<ValueClass> newListALPHA = startingInput.ConvertAll(value => value.Clone());
            // Set the first player to alpha
            newListALPHA[0].team = TEAM.ALPHA;
            // Push it into the result
            result.Add(newListALPHA);
            // And then call the next step
            _Single_Step2(ref result, result.Count - 1, TEAM.ALPHA);
            // And do the same for bravo
            List<ValueClass> newListBRAVO = startingInput.ConvertAll(value => value.Clone());
            newListBRAVO[0].team = TEAM.BRAVO;
            result.Add(newListBRAVO);
            _Single_Step2(ref result, result.Count - 1, TEAM.BRAVO);

            return result;

        }

        // 	2) One player is selected at random from the remaining players and placed in the other team (referred to as Team 2) that wasn't selected in step 1.
        // 
        private void _Single_Step2(ref List<List<ValueClass>> resultsList, int stepIndex, TEAM chosenTeam)
        {
            List<ValueClass> playerList = resultsList[stepIndex];

            // Create a clone of the previous list
            List<ValueClass> oldPlayerList = playerList.ConvertAll(value => value.Clone());

            TEAM nextTeam = (chosenTeam == TEAM.ALPHA) ? TEAM.BRAVO : TEAM.ALPHA;

            // Run through the remaining players (i.e everybody who is of NONE team, and do step 3
            bool isFirst = true;
            for (int i = 0; i < playerList.Count; ++i)
            {
                if (playerList[i].team == TEAM.NONE)
                {
                    if (isFirst)
                    {
                        playerList[i].team = nextTeam;
                        _Single_Step3(ref resultsList, stepIndex);
                        isFirst = false;
                    }
                    else
                    {
                        // Clone the list again
                        List<ValueClass> newList = oldPlayerList.ConvertAll(value => value.Clone());
                        // Set this guy to the other team
                        newList[i].team = nextTeam;
                        // Push it in
                        resultsList.Add(newList);
                        // Send it over to step 3
                        _Single_Step3(ref resultsList, resultsList.Count - 1);
                    }
                    
                }
            }

        }

        // 	3) The remaining players are sorted in Matching Rate order.																									
        // Note: If a number of players have identical Matching Rates then the priority order is decided randomly.
        private void _Single_Step3(ref List<List<ValueClass>> resultsList, int stepIndex)
        {
            List<ValueClass> playerList = resultsList[stepIndex];
            
            // So we just sort em
            playerList.Sort(delegate (ValueClass a, ValueClass b)
            {
                int finalScoreA = a.val;
                if (a.team == TEAM.ALPHA)
                    finalScoreA += 2000000;
                else if (a.team == TEAM.BRAVO)
                    finalScoreA += 200000;

                int finalScoreB = b.val;
                if (b.team == TEAM.ALPHA)
                    finalScoreB += 2000000;
                else if (b.team == TEAM.BRAVO)
                    finalScoreB += 200000;

                return finalScoreB.CompareTo(finalScoreA);
            });

            // And then we go to step 4
            _Single_Step4(ref resultsList, stepIndex);

        }

        // 	4) Moving down the sorted list from the top, the players are placed into the team that has the lower total Matching Rate.							
        // Note: If one team becomes full, the remaining players are placed in the other team.
        // Note: Since the players are assigned teams in order, the total for the Matching Rate(Score) for both teams can become equal.
        // When that happens, the next player to be assigned will be placed into a randomly selected team.

        private void _Single_Step4(ref List<List<ValueClass>> resultsList, int stepIndex)
        {
            List<ValueClass> playerList = resultsList[stepIndex];

            // Recursively set the players, so that it can handle the branching case
            _Single_Step4_Recursive(ref resultsList, stepIndex, 2);
            
        }

        private void _Single_Step4_Recursive(ref List<List<ValueClass>> resultsList, int stepIndex, int currIndex)
        {
            List<ValueClass> playerList = resultsList[stepIndex];
            // If the currIndex is out of index, DONE
            if (currIndex >= playerList.Count())
                return;

            // If not, then just set it up
            // Grab the total score of both teams first
            int currALPHAScore = 0;
            int currBRAVOScore = 0;
            int totalALPHAPlayers = 0;
            int totalBRAVOPlayers = 0;
            for (int i = 0; i < playerList.Count(); ++i)
            {
                if (playerList[i].team == TEAM.ALPHA)
                {
                    currALPHAScore += playerList[i].val;
                    totalALPHAPlayers += 1;
                }
                else if (playerList[i].team == TEAM.BRAVO)
                {
                    currBRAVOScore += playerList[i].val;
                    totalBRAVOPlayers += 1;
                }

            }

            // If one of the teams is filled up, JUST STUFF EM IN AND DONE
            if (totalALPHAPlayers == playerList.Count() / 2)
            {
                foreach (ValueClass val in playerList)
                {
                    if (val.team == TEAM.NONE)
                        val.team = TEAM.BRAVO;
                }
                return;
            }
            else if (totalBRAVOPlayers == playerList.Count() / 2)
            {
                foreach (ValueClass val in playerList)
                {
                    if (val.team == TEAM.NONE)
                        val.team = TEAM.ALPHA;
                }
                return;
            }

            // Then just grab the current index and stuff him in
            if (currALPHAScore > currBRAVOScore)
            {
                playerList[currIndex].team = TEAM.BRAVO;
                _Single_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);
            }
            else if (currALPHAScore < currBRAVOScore)
            {
                playerList[currIndex].team = TEAM.ALPHA;
                _Single_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);

            }
            // OH BOTH ARE THE SAME, RUN BOTH
            else
            {
                // Duplicate the current list first
                List<ValueClass> newList = playerList.ConvertAll(value => value.Clone());
                // Set the old list along its way
                playerList[currIndex].team = TEAM.ALPHA;
                _Single_Step4_Recursive(ref resultsList, stepIndex, currIndex + 1);
                // Then recurse through the new list
                resultsList.Add(newList);
                _Single_Step4_Recursive(ref resultsList, resultsList.Count - 1, currIndex + 1);
            }
        }

        private void clearButton_Click(object sender, EventArgs e)
        {
            resultsBox.Clear();
        }
    }

    public enum TEAM
    {
        ALPHA,
        BRAVO,
        NONE
    }

    public interface IClonable<T>
    {
        T Clone();
    }

    // USED FOR THE SCORE OR MMR CASE
    public class ValueClass : IClonable<ValueClass>
    {
        public int val;

        public int realScore;
        public int realMMR;

        public TEAM team;

        public ValueClass Clone()
        {
            ValueClass clonedVal = new ValueClass();
            clonedVal.val = val;
            clonedVal.realScore = realScore;
            clonedVal.realMMR = realMMR;
            clonedVal.team = team;
            return clonedVal;
        }

        public DoubleValueClass ConvertToDoubleValue()
        {
            DoubleValueClass dbl = new DoubleValueClass();
            dbl.score = realScore;
            dbl.mmr = realMMR;
            dbl.team = team;
            return dbl;
        }

        public string Print()
        {
                return team.ToString() + ":" + val.ToString();
        }
    }

    // USED FOR THE SCORE AND MMR CASE
    public class DoubleValueClass : IClonable<DoubleValueClass>
    {
        public int score;
        public int mmr;
        public TEAM team;

        public DoubleValueClass Clone()
        {
            DoubleValueClass clonedVal = new DoubleValueClass();
            clonedVal.score = score;
            clonedVal.mmr = mmr;
            clonedVal.team = team;
            return clonedVal;
        }

        public string Print()
        {
            string finalString = team.ToString() + ":";
            if (score >= 0)
                finalString += "S=" + score.ToString();
            finalString += "MMR=" + mmr.ToString();
            return finalString;
        }
    }

    public class Player : IClonable<Player>
    {
        public string name;
        public int mmr;
        public int score;
        public TEAM team;

        // DON'T TOUCH THIS, this is the comparing value
        public int cVal;

        public Player Clone()
        {
            Player clonedPlayer = new Player();
            clonedPlayer.name = name;
            clonedPlayer.mmr = mmr;
            clonedPlayer.score = score;
            clonedPlayer.team = team;
            clonedPlayer.cVal = cVal;
            return clonedPlayer;
        }

        public ValueClass ConvertToValue(bool usingScore)
        {
            ValueClass newVal = new ValueClass();
            newVal.val = usingScore ? score : mmr;
            newVal.realScore = score;
            newVal.realMMR = mmr;
            newVal.team = team;
            return newVal;
        }

        public DoubleValueClass ConvertToDoubleValue()
        {
            DoubleValueClass dbl = new DoubleValueClass();
            dbl.score = score;
            dbl.mmr = mmr;
            dbl.team = team;
            return dbl;
        }

    }
    
}
