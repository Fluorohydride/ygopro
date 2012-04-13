--先史遺産モアイ
function c87430304.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_SPSUM_PARAM)
	e1:SetRange(LOCATION_HAND)
	e1:SetTargetRange(POS_FACEUP_DEFENCE,0)
	e1:SetCondition(c87430304.hspcon)
	c:RegisterEffect(e1)
end
function c87430304.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x70)
end
function c87430304.hspcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c87430304.cfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
