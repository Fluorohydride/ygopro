--水面のアレサ
function c22377815.initial_effect(c)
	aux.EnableDualAttribute(c)
	--handes
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(22377815,0))
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(c22377815.con)
	e1:SetTarget(c22377815.tg)
	e1:SetOperation(c22377815.op)
	c:RegisterEffect(e1)
end
function c22377815.con(e,tp,eg,ep,ev,re,r,rp)
	return aux.IsDualState(e) and aux.bdogcon(e,tp,eg,ep,ev,re,r,rp)
end
function c22377815.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,1-tp,1)
end
function c22377815.op(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g:GetCount()==0 then return end
	local sg=g:RandomSelect(1-tp,1)
	Duel.SendtoGrave(sg,REASON_DISCARD+REASON_EFFECT)
end
