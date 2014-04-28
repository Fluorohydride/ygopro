--魂を削る死霊
function c23205979.initial_effect(c)
	--battle indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--handes
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(23205979,0))
	e2:SetCategory(CATEGORY_HANDES)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLE_DAMAGE)
	e2:SetCondition(c23205979.condition)
	e2:SetTarget(c23205979.target)
	e2:SetOperation(c23205979.operation)
	c:RegisterEffect(e2)
	--self des
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_SELF_DESTROY)
	e3:SetCondition(c23205979.sdcon)
	c:RegisterEffect(e3)
	--be target
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e4:SetCode(EVENT_BECOME_TARGET)
	e4:SetOperation(c23205979.desop1)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCode(EVENT_CHAIN_SOLVED)
	e5:SetOperation(c23205979.desop2)
	e5:SetLabelObject(e4)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EVENT_BATTLED)
	e6:SetOperation(c23205979.desop3)
	e6:SetLabelObject(e4)
	c:RegisterEffect(e6)
end
function c23205979.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and Duel.GetAttackTarget()==nil
end
function c23205979.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,1-tp,1)
end
function c23205979.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(ep,LOCATION_HAND,0)
	if g:GetCount()==0 then return end
	local sg=g:RandomSelect(1-tp,1)
	Duel.SendtoGrave(sg,REASON_DISCARD+REASON_EFFECT)
end
function c23205979.sdcon(e)
	return e:GetHandler():GetOwnerTargetCount()>0
end
function c23205979.desop1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsLocation(LOCATION_MZONE) and c:IsFaceup() then
		e:SetLabelObject(re)
		e:SetLabel(0)
	end
end
function c23205979.desop2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if re==e:GetLabelObject():GetLabelObject() and c:IsRelateToEffect(re) then
		if Duel.GetCurrentPhase()==PHASE_DAMAGE and not Duel.IsDamageCalculated() then
			e:GetLabelObject():SetLabel(1)
		else
			if c:IsHasEffect(EFFECT_DISABLE) then return end
			if not c:IsDisabled() then Duel.Destroy(c,REASON_EFFECT) end
		end
	end
end
function c23205979.desop3(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local des=e:GetLabelObject():GetLabel()
	e:GetLabelObject():SetLabel(0)
	if c:IsHasEffect(EFFECT_DISABLE) then return end
	if des==1 and not c:IsDisabled() then
		Duel.Destroy(c,REASON_EFFECT)
	end
end
