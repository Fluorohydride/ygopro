--マシンナーズ・カノン
function c39284521.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c39284521.spcon)
	e1:SetOperation(c39284521.spop)
	c:RegisterEffect(e1)
end
function c39284521.spfilter(c)
	return c:IsRace(RACE_MACHINE) and c:IsAbleToGraveAsCost()
end
function c39284521.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c39284521.spfilter,c:GetControler(),LOCATION_HAND,0,1,c)
end
function c39284521.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c39284521.spfilter,c:GetControler(),LOCATION_HAND,0,1,63,c)
	Duel.SendtoGrave(g,REASON_COST)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(g:GetCount()*800)
	e1:SetReset(RESET_EVENT+0xff0000)
	c:RegisterEffect(e1)
end
