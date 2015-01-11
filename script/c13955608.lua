--機動砦 ストロング・ホールド
function c13955608.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c13955608.target)
	e1:SetOperation(c13955608.activate)
	c:RegisterEffect(e1)
end
function c13955608.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,13955608,0,0x21,0,2000,4,RACE_MACHINE,ATTRIBUTE_EARTH) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c13955608.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,13955608,0,0x21,0,2000,4,RACE_MACHINE,ATTRIBUTE_EARTH) then return end
	c:AddTrapMonsterAttribute(TYPE_EFFECT,ATTRIBUTE_EARTH,RACE_MACHINE,4,0,2000)
	Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP_DEFENCE)
	c:TrapMonsterBlock()
	--cannot attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetValue(3000)
	e1:SetCondition(c13955608.atkcon)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e1)
end
function c13955608.atkcon(e)
	local con=0
	for i=0,4 do
		local tc=Duel.GetFieldCard(e:GetHandlerPlayer(),LOCATION_MZONE,i)
		if tc and tc:IsFaceup() then
			local code=tc:GetCode()
			if code==13839120 then con=bit.bor(con,1)
			elseif code==86445415 then con=bit.bor(con,2)
			elseif code==41172955 then con=bit.bor(con,4)
			end
			if con==7 then return true end
		end
	end
	return false
end
