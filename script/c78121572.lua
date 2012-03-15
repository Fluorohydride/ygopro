--黒魔力の精製者
function c78121572.initial_effect(c)
	--counter
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(78121572,0))
	e1:SetCategory(CATEGORY_COUNTER)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c78121572.con)
	e1:SetTarget(c78121572.tg)
	e1:SetOperation(c78121572.op)
	c:RegisterEffect(e1)
end
function c78121572.con(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPosition(POS_FACEUP_ATTACK)
end
function c78121572.filter(c)
	return c:IsFaceup() and c:IsCanAddCounter(0x3001,1)
end
function c78121572.tg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(tp) and c78121572.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c78121572.filter,tp,LOCATION_ONFIELD,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(78121572,1))
	local g=Duel.SelectTarget(tp,c78121572.filter,tp,LOCATION_ONFIELD,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_COUNTER,nil,1,0,0x3001)
end
function c78121572.op(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsPosition(POS_FACEUP_ATTACK) then
		Duel.ChangePosition(c,POS_FACEUP_DEFENCE)
		if tc:IsRelateToEffect(e) then
			tc:AddCounter(0x3001,1)
		end
	end
end
