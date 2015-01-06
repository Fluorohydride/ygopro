--ゴッドハンド・スマッシュ
function c97570038.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c97570038.condition)
	e1:SetOperation(c97570038.operation)
	c:RegisterEffect(e1)
end
function c97570038.filter(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==8508055 or code==3810071 or code==49814180)
end
function c97570038.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c97570038.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c97570038.operation(e,tp,eg,ep,ev,re,r,rp)
	--destroy
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetOperation(c97570038.desop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c97570038.cfilter(c)
	local code=c:GetCode()
	return code==8508055 or code==3810071 or code==49814180
end
function c97570038.desop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	if not at then return end
	local g=Group.CreateGroup()
	if c97570038.cfilter(a) and at:IsLocation(LOCATION_MZONE) then g:AddCard(at) end
	if c97570038.cfilter(at) and a:IsLocation(LOCATION_MZONE) then g:AddCard(a) end
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
