--ブラック・サンダー
function c52833089.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c52833089.condition)
	e1:SetTarget(c52833089.target)
	e1:SetOperation(c52833089.activate)
	c:RegisterEffect(e1)
end
function c52833089.cfilter(c,tp)
	return c:IsSetCard(0x33) and c:IsLocation(LOCATION_GRAVE)
		and c:GetPreviousControler()==tp and bit.band(c:GetReason(),REASON_BATTLE)~=0
end
function c52833089.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c52833089.cfilter,1,nil,tp)
end
function c52833089.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_ONFIELD)>0 end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,400)
end
function c52833089.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local d=Duel.GetFieldGroupCount(p,LOCATION_ONFIELD,0)*400
	Duel.Damage(p,d,REASON_EFFECT)
end
