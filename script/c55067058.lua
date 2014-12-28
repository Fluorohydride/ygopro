--No.19 フリーザードン
function c55067058.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,5,2)
	c:EnableReviveLimit()
	--remove overlay replace
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(55067058,0))
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_OVERLAY_REMOVE_REPLACE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c55067058.rcon)
	e1:SetOperation(c55067058.rop)
	c:RegisterEffect(e1)
end
c55067058.xyz_number=19
function c55067058.rcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_COST)~=0 and re:IsHasType(0x7e0) and re:IsActiveType(TYPE_XYZ)
		and e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_EFFECT)
		and ep==e:GetOwnerPlayer() and re:GetHandler():GetOverlayCount()>=ev-1
end
function c55067058.rop(e,tp,eg,ep,ev,re,r,rp)
	local ct=bit.band(ev,0xffff)
	if ct==1 then
		e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_EFFECT)
	else
		e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_EFFECT)
		re:GetHandler():RemoveOverlayCard(tp,ct-1,ct-1,REASON_COST)
	end
end
