--고신 하스톨
function c70913714.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--equip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(70913714,0))
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c70913714.eqcon)
	e1:SetTarget(c70913714.eqtg)
	e1:SetOperation(c70913714.eqop)
	c:RegisterEffect(e1)
end
function c70913714.eqcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_MZONE)
end
function c70913714.eqtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsFaceup() end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil)
end
function c70913714.eqlimit(e,c)
	return e:GetOwner()==c
end
function c70913714.eqop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Equip(tp,c,tc,true)
		--Add Equip limit
		local e1=Effect.CreateEffect(tc)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EQUIP_LIMIT)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(c70913714.eqlimit)
		c:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_EQUIP)
		e2:SetCode(EFFECT_DISABLE)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e2)
		local e3=e2:Clone()
		e3:SetCode(EFFECT_CANNOT_ATTACK)
		c:RegisterEffect(e3)
		--control
		local e4=Effect.CreateEffect(c)
		e4:SetDescription(aux.Stringid(70913714,1))
		e4:SetCategory(CATEGORY_CONTROL)
		e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e4:SetCode(EVENT_LEAVE_FIELD)
		e4:SetTarget(c70913714.cttg)
		e4:SetOperation(c70913714.ctop)
		e4:SetReset(RESET_EVENT+0x1020000)
		c:RegisterEffect(e4)
	end
end
function c70913714.cttg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ec=e:GetHandler():GetPreviousEquipTarget()
	if ec:IsLocation(LOCATION_MZONE) and ec:IsControlerCanBeChanged() then
		Duel.SetTargetCard(ec)
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,ec,1,0,0)
	end
end
function c70913714.ctop(e,tp,eg,ep,ev,re,r,rp)
	local ec=Duel.GetFirstTarget()
	if ec and ec:IsRelateToEffect(e) and not Duel.GetControl(ec,tp) then
		if not ec:IsImmuneToEffect(e) and ec:IsAbleToChangeControler() then
			Duel.Destroy(ec,REASON_EFFECT)
		end
	end
end
