--幻水龍
function c92841002.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c92841002.spcon)
	e1:SetOperation(c92841002.spop)
	c:RegisterEffect(e1)
end
function c92841002.spfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_EARTH)
end
function c92841002.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.GetFlagEffect(tp,92841002)==0
		and Duel.IsExistingMatchingCard(c92841002.spfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c92841002.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.RegisterFlagEffect(tp,92841002,RESET_PHASE+PHASE_END,0,1)
end
