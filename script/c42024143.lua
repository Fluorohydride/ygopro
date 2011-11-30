--XX－セイバー ガルドストライク
function c42024143.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c42024143.spcon)
	c:RegisterEffect(e1)
end
function c42024143.spfilter(c)
	return c:IsSetCard(0x100d) and c:IsType(TYPE_MONSTER)
end
function c42024143.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.GetFieldGroupCount(c:GetControler(),LOCATION_MZONE,0)==0
		and Duel.IsExistingMatchingCard(c42024143.spfilter,c:GetControler(),LOCATION_GRAVE,0,2,nil)
end
