--ギミック・パペット－マグネ・ドール
function c39806198.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c39806198.spcon)
	c:RegisterEffect(e1)
end
function c39806198.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)>0
		and	Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0
		and not Duel.IsExistingMatchingCard(c39806198.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c39806198.cfilter(c)
	return c:IsFacedown() or not c:IsSetCard(0x83)
end
