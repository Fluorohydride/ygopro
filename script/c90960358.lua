--トゥーン・ブラック・マジシャン・ガール
function c90960358.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c90960358.splimit)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c90960358.spcon)
	e2:SetOperation(c90960358.spop)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c90960358.sdescon)
	e3:SetOperation(c90960358.sdesop)
	c:RegisterEffect(e3)
	--direct attack
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_DIRECT_ATTACK)
	e4:SetCondition(c90960358.dircon)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e5:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e5:SetRange(LOCATION_MZONE)
	e5:SetTargetRange(0,LOCATION_MZONE)
	e5:SetTarget(c90960358.attg)
	e5:SetCondition(c90960358.atcon)
	e5:SetValue(c90960358.atval)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e6:SetCondition(c90960358.atcon)
	c:RegisterEffect(e6)
	--cannot attack
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e7:SetCode(EVENT_SUMMON_SUCCESS)
	e7:SetOperation(c90960358.atklimit)
	c:RegisterEffect(e7)
	--atkup
	local e8=Effect.CreateEffect(c)
	e8:SetType(EFFECT_TYPE_SINGLE)
	e8:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e8:SetCode(EFFECT_UPDATE_ATTACK)
	e8:SetRange(LOCATION_MZONE)
	e8:SetValue(c90960358.val)
	c:RegisterEffect(e8)
end
function c90960358.splimit(e,se,sp,st,spos,tgp)
	return Duel.IsExistingMatchingCard(c90960358.cfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c90960358.cfilter(c)
	return c:IsFaceup() and c:IsCode(15259703)
end
function c90960358.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	if not Duel.IsExistingMatchingCard(c90960358.cfilter,tp,LOCATION_ONFIELD,0,1,nil) then return false end
	local lv=c:GetLevel()
	if lv<5 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
	elseif lv<7 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1 and Duel.CheckReleaseGroup(tp,nil,1,nil)
	else return Duel.GetLocationCount(tp,LOCATION_MZONE)>-2 and Duel.CheckReleaseGroup(tp,nil,2,nil) end
end
function c90960358.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local lv=c:GetLevel()
	local tp=c:GetControler()
	if lv<5 then 
	elseif lv<7 then
		local g=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
		Duel.Release(g,REASON_COST)
	else
		local g=Duel.SelectReleaseGroup(tp,nil,2,2,nil)
		Duel.Release(g,REASON_COST)
	end
end
function c90960358.sfilter(c)
	return c:IsReason(REASON_DESTROY) and c:IsCode(15259703) and c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c90960358.sdescon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c90960358.sfilter,1,nil)
end
function c90960358.sdesop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
function c90960358.atkfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TOON)
end
function c90960358.dircon(e)
	return not Duel.IsExistingMatchingCard(c90960358.atkfilter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c90960358.atcon(e)
	return Duel.IsExistingMatchingCard(c90960358.atkfilter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c90960358.attg(e,c)
	return not c:IsType(TYPE_TOON)
end
function c90960358.atval(e,c)
	return c==e:GetHandler()
end
function c90960358.atklimit(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
end
function c90960358.val(e,c)
	return Duel.GetMatchingGroupCount(c90960358.filter,c:GetControler(),LOCATION_GRAVE,LOCATION_GRAVE,nil)*300
end
function c90960358.filter(c)
	local code=c:GetCode()
	return code==46986414 or code==30208479
end
