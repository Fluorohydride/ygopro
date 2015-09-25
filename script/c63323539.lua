--トロイボム
function c63323539.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CONTROL_CHANGED)
	e1:SetTarget(c63323539.target)
	e1:SetOperation(c63323539.operation)
	c:RegisterEffect(e1)
end
function c63323539.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return r==REASON_EFFECT and rp==1-tp
		and eg:IsExists(Card.IsControler,1,nil,1-tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c63323539.filter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsControler(1-tp)
end
function c63323539.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=eg:FilterSelect(tp,c63323539.filter,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if not tc then return end
	local atk=tc:GetAttack()
	if atk<0 or tc:IsFacedown() then atk=0 end
	if Duel.Destroy(tc,REASON_EFFECT)~=0 then
		Duel.Damage(1-tp,atk,REASON_EFFECT)
	end
end
