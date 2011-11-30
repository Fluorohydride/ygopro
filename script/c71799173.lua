--ガーディアン·オブ·オーダー
function c71799173.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c71799173.spcon)
	c:RegisterEffect(e1)
	--only 1 can exist
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	e2:SetCondition(c71799173.excon)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e4:SetCode(EFFECT_SPSUMMON_CONDITION)
	e4:SetValue(c71799173.splimit)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_SELF_DESTROY)
	e5:SetCondition(c71799173.descon)
	c:RegisterEffect(e5)
end
function c71799173.spfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c71799173.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and	Duel.IsExistingMatchingCard(c71799173.spfilter,c:GetControler(),LOCATION_MZONE,0,2,nil)
		and not Duel.IsExistingMatchingCard(c71799173.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c71799173.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==71799173 and (fid==nil or c:GetFieldID()<fid)
end
function c71799173.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c71799173.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c71799173.splimit(e,se,sp,st,spos,tgp)
	return not Duel.IsExistingMatchingCard(c71799173.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c71799173.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c71799173.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
