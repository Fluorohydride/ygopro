--A・ジェネクス・ドゥルダーク
function c68450517.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(68450517,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c68450517.descost)
	e1:SetTarget(c68450517.destg)
	e1:SetOperation(c68450517.desop)
	c:RegisterEffect(e1)
end
function c68450517.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetAttackAnnouncedCount()==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
	e:GetHandler():RegisterEffect(e1)
end
function c68450517.filter(c,att)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsAttribute(att) and c:IsDestructable()
end
function c68450517.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp)
		and c68450517.filter(chkc,e:GetHandler():GetAttribute()) end
	if chk==0 then return Duel.IsExistingTarget(c68450517.filter,tp,0,LOCATION_MZONE,1,nil,e:GetHandler():GetAttribute()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c68450517.filter,tp,0,LOCATION_MZONE,1,1,nil,e:GetHandler():GetAttribute())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c68450517.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc:IsRelateToEffect(e) and tc:IsControler(1-tp) and c68450517.filter(tc,c:GetAttribute()) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
