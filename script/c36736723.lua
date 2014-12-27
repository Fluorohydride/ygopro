--ラッシュ・ウォリアー
function c36736723.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(36736723,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,36736723)
	e1:SetCondition(c36736723.atkcon)
	e1:SetCost(c36736723.atkcost)
	e1:SetOperation(c36736723.atkop)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(36736723,1))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,36736724)
	e2:SetCost(c36736723.thcost)
	e2:SetTarget(c36736723.thtg)
	e2:SetOperation(c36736723.thop)
	c:RegisterEffect(e2)
end
function c36736723.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttackTarget()
	if not tc then return false end
	if tc:IsControler(1-tp) then tc=Duel.GetAttacker() end
	e:SetLabelObject(tc)
	return tc and tc:IsRelateToBattle() and tc:IsSetCard(0x66) and tc:IsType(TYPE_SYNCHRO)
end
function c36736723.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c36736723.atkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:IsFaceup() and tc:IsRelateToBattle() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE_CAL)
		e1:SetValue(tc:GetAttack()*2)
		tc:RegisterEffect(e1)
	end
end
function c36736723.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c36736723.filter(c)
	return c:IsSetCard(0x17) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c36736723.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c36736723.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c36736723.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c36736723.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c36736723.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
