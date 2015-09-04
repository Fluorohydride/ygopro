--ジュラック・プティラ
function c45711266.initial_effect(c)
	--tohand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(45711266,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLED)
	e1:SetCondition(c45711266.condition)
	e1:SetTarget(c45711266.target)
	e1:SetOperation(c45711266.operation)
	c:RegisterEffect(e1)
end
function c45711266.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler()==Duel.GetAttackTarget()
end
function c45711266.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local tc=Duel.GetAttacker()
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tc,1,0,0)
end
function c45711266.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetAttacker()
	if tc:IsRelateToBattle() then
		local lv=tc:GetLevel()
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		if not c:IsStatus(STATUS_BATTLE_DESTROYED) then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_DEFENCE)
			e1:SetValue(lv*100)
			e1:SetReset(RESET_EVENT+0x1ff0000)
			c:RegisterEffect(e1)
		end
	end
end
