--クローン複製
function c86871614.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c86871614.condition)
	e1:SetTarget(c86871614.target)
	e1:SetOperation(c86871614.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
end
function c86871614.condition(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	return ep~=tp
end
function c86871614.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local ec=eg:GetFirst()
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,86871615,0,0x4011,ec:GetBaseAttack(),ec:GetBaseDefence(),
			ec:GetOriginalLevel(),ec:GetOriginalRace(),ec:GetOriginalAttribute()) end
	ec:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c86871614.activate(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	if not ec:IsRelateToEffect(e) or ec:IsFacedown() then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,86871615,0,0x4011,ec:GetBaseAttack(),ec:GetBaseDefence(),
			ec:GetOriginalLevel(),ec:GetOriginalRace(),ec:GetOriginalAttribute()) then return end
	ec:RegisterFlagEffect(86871614,RESET_EVENT+0x17a0000,0,0)
	local token=Duel.CreateToken(tp,86871615)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetValue(ec:GetBaseAttack())
	e1:SetReset(RESET_EVENT+0xfe0000)
	token:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_SET_BASE_DEFENCE)
	e2:SetValue(ec:GetBaseDefence())
	token:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EFFECT_CHANGE_LEVEL)
	e3:SetValue(ec:GetOriginalLevel())
	token:RegisterEffect(e3)
	local e4=e1:Clone()
	e4:SetCode(EFFECT_CHANGE_RACE)
	e4:SetValue(ec:GetOriginalRace())
	token:RegisterEffect(e4)
	local e5=e1:Clone()
	e5:SetCode(EFFECT_CHANGE_ATTRIBUTE)
	e5:SetValue(ec:GetOriginalAttribute())
	token:RegisterEffect(e5)
	Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
	local de=Effect.CreateEffect(e:GetHandler())
	de:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	de:SetRange(LOCATION_MZONE)
	de:SetCode(EVENT_TO_GRAVE)
	de:SetCondition(c86871614.descon)
	de:SetOperation(c86871614.desop)
	de:SetReset(RESET_EVENT+0xfe0000)
	token:RegisterEffect(de)
end
function c86871614.dfilter(c)
	return c:IsReason(REASON_DESTROY) and c:GetFlagEffect(86871614)~=0
end
function c86871614.descon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c86871614.dfilter,1,nil)
end
function c86871614.desop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
