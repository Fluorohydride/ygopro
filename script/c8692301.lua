--ジェムナイト・ジルコニア
function c8692301.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcFun2(c,aux.FilterBoolFunction(Card.IsSetCard,0x1047),aux.FilterBoolFunction(Card.IsRace,RACE_ROCK),true)
end
