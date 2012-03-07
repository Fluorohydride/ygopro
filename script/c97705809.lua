--スーパーチャージ
function c97705809.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c97705809.condition)
	e1:SetTarget(c97705809.target)
	e1:SetOperation(c97705809.activate)
	c:RegisterEffect(e1)
end
function c97705809.cfilter(c)
	return c:IsFacedown() or not c:IsSetCard(0x16) or not c:IsRace(RACE_MACHINE)
end
function c97705809.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)~=0
		and not Duel.IsExistingMatchingCard(c97705809.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c97705809.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(2)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c97705809.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
