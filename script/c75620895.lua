--隻眼のスキル・ゲイナー
function c75620895.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,3)
	c:EnableReviveLimit()
	--attack up
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetDescription(aux.Stringid(75620895,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_NO_TURN_RESET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c75620895.cost)
	e1:SetTarget(c75620895.target)
	e1:SetOperation(c75620895.operation)
	c:RegisterEffect(e1)
end
function c75620895.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c75620895.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c75620895.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) and c75620895.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c75620895.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c75620895.filter,tp,0,LOCATION_MZONE,1,1,nil)
end
function c75620895.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and c:IsRelateToEffect(e) and c:IsFaceup() and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local code=tc:GetOriginalCode()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCode(EFFECT_CHANGE_CODE)
		e1:SetValue(code)
		c:RegisterEffect(e1)
		c:CopyEffect(code,RESET_EVENT+0x1fe0000,1)
	end
end
