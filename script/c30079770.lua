--真紅眼の黒炎竜
function c30079770.initial_effect(c)
	aux.EnableDualAttribute(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCountLimit(1,30079770)
	e1:SetCondition(c30079770.damcon)
	e1:SetTarget(c30079770.damtg)
	e1:SetOperation(c30079770.damop)
	c:RegisterEffect(e1)
end
function c30079770.damcon(e,tp,eg,ep,ev,re,r,rp)
	return aux.IsDualState(e) and e:GetHandler():GetBattledGroupCount()>0
end
function c30079770.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local atk=e:GetHandler():GetBaseAttack()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,atk)
end
function c30079770.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local atk=c:GetBaseAttack()
		Duel.Damage(1-tp,atk,REASON_EFFECT)
	end
end
