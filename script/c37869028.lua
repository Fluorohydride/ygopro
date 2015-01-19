--トリプル・ヴァイパー
function c37869028.initial_effect(c)
	--multiattack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(2)
	c:RegisterEffect(e1)
	--attack cost
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_ATTACK_COST)
	e2:SetCost(c37869028.atcost)
	e2:SetOperation(c37869028.atop)
	c:RegisterEffect(e2)
end
function c37869028.atcost(e,c,tp)
	return Duel.CheckReleaseGroup(tp,Card.IsRace,1,nil,RACE_AQUA)
end
function c37869028.atop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.SelectReleaseGroup(tp,Card.IsRace,1,1,nil,RACE_AQUA)
	Duel.Release(g,REASON_COST)
end
