--ラプターズ・アルティメット・メイス
function c87609391.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c87609391.target)
	e1:SetOperation(c87609391.operation)
	c:RegisterEffect(e1)
	--atk up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_EQUIP)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(1000)
	c:RegisterEffect(e2)
	--equip limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_EQUIP_LIMIT)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetValue(c87609391.eqlimit)
	c:RegisterEffect(e3)
	--search
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_BE_BATTLE_TARGET)
	e4:SetCondition(c87609391.thcon)
	e4:SetTarget(c87609391.thtg)
	e4:SetOperation(c87609391.thop)
	c:RegisterEffect(e4)
end
function c87609391.eqlimit(e,c)
	return c:IsSetCard(0xba)
end
function c87609391.filter(c)
	return c:IsFaceup() and c:IsSetCard(0xba)
end
function c87609391.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c87609391.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c87609391.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c87609391.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c87609391.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if e:GetHandler():IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,e:GetHandler(),tc)
	end
end
function c87609391.thcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local ec=e:GetHandler():GetEquipTarget()
	local at=Duel.GetAttacker()
	return tc==ec and at and at:GetAttack()>ec:GetAttack()
end
function c87609391.thfilter(c)
	return c:IsSetCard(0x95) and c:IsType(TYPE_SPELL) and c:IsAbleToHand()
end
function c87609391.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c87609391.thfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c87609391.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c87609391.thfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
		local c=e:GetHandler()
		local tc=Duel.GetAttacker()
		if tc:IsRelateToBattle() and tc:IsFaceup() and tc:IsAttackable() then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
			e1:SetValue(1)
			e1:SetCondition(c87609391.damcon)
			e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
			e1:SetLabelObject(tc)
			c:GetEquipTarget():RegisterEffect(e1,true)
		end
	end
end
function c87609391.damcon(e)
	return e:GetLabelObject()==Duel.GetAttacker()
end
