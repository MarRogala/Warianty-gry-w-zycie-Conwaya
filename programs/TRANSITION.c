skipState(0, 1.0);
cond1 = count(0, 1.0) > 0;
cond2 = random(0.0, 1.0) > 0.5;
if(cond1 && cond2)
{
    newState[0] = 1;
}
if(cond1 && (cond == 0.0))
{
    newState[1] = state[1] + 1;
}
