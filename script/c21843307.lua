--コピー・ナイト
function c21843307.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c21843307.condition)
	e1:SetTarget(c21843307.target)
	e1:SetOperation(c21843307.activate)
	c:RegisterEffect(e1)
end
function c21843307.condition(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	return ep==tp and ec:IsLevelBelow(4) and ec:IsRace(RACE_WARRIOR)
end
function c21843307.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local ec=eg:GetFirst()
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,ec:GetCode(),0,0x11,0,0,ec:GetLevel(),RACE_WARRIOR,ATTRIBUTE_LIGHT) end
	ec:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c21843307.activate(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	if not ec:IsRelateToEffect(e) then return end
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,ec:GetCode(),0,0x11,0,0,ec:GetLevel(),RACE_WARRIOR,ATTRIBUTE_LIGHT) then return end
	c:AddTrapMonsterAttribute(TYPE_NORMAL,ATTRIBUTE_LIGHT,RACE_WARRIOR,ec:GetLevel(),0,0)
	Duel.SpecialSummonStep(c,0,tp,tp,true,false,POS_FACEUP)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CHANGE_CODE)
	e1:SetValue(ec:GetCode())
	e1:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e1)
	Duel.SpecialSummonComplete()
	c:TrapMonsterBlock()
end
