--ヘル・ブラスト
function c18271561.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCondition(c18271561.condition)
	e1:SetTarget(c18271561.target)
	e1:SetOperation(c18271561.operation)
	c:RegisterEffect(e1)
end
function c18271561.filter(c,tp)
	local pos=c:GetPreviousPosition()
	if c:IsReason(REASON_BATTLE) then pos=c:GetBattlePosition() end
	return c:IsPreviousLocation(LOCATION_MZONE) and bit.band(pos,POS_FACEUP)~=0
		and c:GetPreviousControler()==tp and c:IsReason(REASON_DESTROY)
end
function c18271561.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	return eg:IsExists(c18271561.filter,1,nil,tp)
end
function c18271561.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local dg=g:GetMinGroup(Card.GetAttack)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,0)
end
function c18271561.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	local dg=g:GetMinGroup(Card.GetAttack)
	if dg:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		dg=dg:Select(tp,1,1,nil)
	end
	local atk=dg:GetFirst():GetAttack()/2
	if Duel.Destroy(dg,REASON_EFFECT)>0 then
		Duel.Damage(tp,atk,REASON_EFFECT)
		Duel.Damage(1-tp,atk,REASON_EFFECT)
	end
end
