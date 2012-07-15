--覚醒の勇士 ガガギゴ
function c9053187.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterEqualFunction(Card.GetLevel,4),3)
	c:EnableReviveLimit()
end
