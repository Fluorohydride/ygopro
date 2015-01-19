--プリベント・スター
function c94303232.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c94303232.target)
	e1:SetOperation(c94303232.operation)
	c:RegisterEffect(e1)
	--Equip limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_EQUIP_LIMIT)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetValue(c94303232.eqlimit)
	c:RegisterEffect(e2)
	--Remove
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(94303232,0))
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c94303232.rmcon)
	e3:SetTarget(c94303232.rmtg)
	e3:SetOperation(c94303232.rmop)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
	if not c94303232.global_check then
		c94303232.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CHANGE_POS)
		ge1:SetOperation(c94303232.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c94303232.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsPreviousPosition(POS_FACEUP_ATTACK) and tc:IsPosition(POS_FACEUP_DEFENCE) then
			tc:RegisterFlagEffect(94303232,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		end
		tc=eg:GetNext()
	end
end
function c94303232.eqlimit(e,c)
	return c:GetFlagEffect(94303232)~=0 or c:GetFlagEffect(94303233)~=0
end
function c94303232.filter(c)
	return c:IsFaceup() and c:GetFlagEffect(94303232)~=0
end
function c94303232.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c94303232.filter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(nil,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c94303232.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_OPPO)
	local g=Duel.SelectTarget(tp,nil,tp,0,LOCATION_MZONE,1,1,nil)
	e:SetLabelObject(g:GetFirst())
end
function c94303232.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local dc=e:GetLabelObject()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	if dc==tc then tc=g:GetNext() end
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		if Duel.Equip(tp,c,tc) and dc:IsRelateToEffect(e) then
			tc:RegisterFlagEffect(94303233,RESET_EVENT+0x1fe0000,0,1)
			c:SetCardTarget(dc)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_OWNER_RELATE)
			e1:SetRange(LOCATION_MZONE)
			e1:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			e1:SetCondition(c94303232.rcon)
			dc:RegisterEffect(e1,true)
			local e2=e1:Clone()
			e2:SetCode(EFFECT_CANNOT_ATTACK)
			dc:RegisterEffect(e2,true)
		end
	end
end
function c94303232.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
function c94303232.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_LOST_TARGET) and c:GetPreviousEquipTarget():IsReason(REASON_DESTROY)
		and c:IsHasCardTarget(e:GetLabelObject():GetLabelObject())
end
function c94303232.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local tc=e:GetLabelObject():GetLabelObject()
	Duel.SetTargetCard(tc)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,tc,1,0,0)
end
function c94303232.rmop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
