--メタル・リフレクト・スライム
function c26905245.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c26905245.target)
	e1:SetOperation(c26905245.activate)
	c:RegisterEffect(e1)
end
function c26905245.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,26905245,0,0x21,0,3000,10,RACE_AQUA,ATTRIBUTE_WATER) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c26905245.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,26905245,0,0x21,0,3000,10,RACE_AQUA,ATTRIBUTE_WATER) then return end
	c:AddTrapMonsterAttribute(TYPE_EFFECT,ATTRIBUTE_WATER,RACE_AQUA,10,0,3000)
	Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP_DEFENCE)
	c:TrapMonsterBlock()
	--cannot attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e1)
end
