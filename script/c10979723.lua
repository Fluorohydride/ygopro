--アマゾネスペット虎
function c10979723.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c10979723.val)
	c:RegisterEffect(e1)
	--at limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e2:SetTarget(c10979723.atlimit)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--only 1 can exists
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c10979723.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c10979723.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c10979723.descon)
	c:RegisterEffect(e6)
end
function c10979723.val(e,c)
	return Duel.GetMatchingGroupCount(c10979723.filter,c:GetControler(),LOCATION_MZONE,0,nil)*400
end
function c10979723.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x4)
end
function c10979723.atlimit(e,c)
	return c:IsSetCard(0x4) and c~=e:GetHandler()
end
function c10979723.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==10979723 and (fid==nil or c:GetFieldID()<fid)
end
function c10979723.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c10979723.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c10979723.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c10979723.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c10979723.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c10979723.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
