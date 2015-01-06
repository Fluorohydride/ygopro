--マザー・スパイダー
function c17021204.initial_effect(c)
	--spsummon proc
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c17021204.spcon)
	e1:SetOperation(c17021204.spop)
	c:RegisterEffect(e1)
end
function c17021204.spfilter(c)
	return c:IsPosition(POS_FACEUP_DEFENCE) and c:IsAbleToGraveAsCost()
end
function c17021204.check(tp)
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_GRAVE,0,nil,TYPE_MONSTER)
	return g:GetCount()~=0 and not g:IsExists(Card.IsRace,1,nil,0xfffffff-RACE_INSECT)
end
function c17021204.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c17021204.spfilter,c:GetControler(),0,LOCATION_MZONE,2,nil)
		and c17021204.check(c:GetControler())
end
function c17021204.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c17021204.spfilter,tp,0,LOCATION_MZONE,2,2,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
